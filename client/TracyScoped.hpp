#ifndef __TRACYSCOPED_HPP__
#define __TRACYSCOPED_HPP__

#include <stdint.h>
#include <string.h>

#include "../common/TracySystem.hpp"
#include "TracyProfiler.hpp"

namespace tracy
{

class ScopedZone
{
public:
    tracy_force_inline ScopedZone( const SourceLocation* srcloc )
    {
        const auto thread = GetThreadHandle();
        m_thread = thread;
        Magic magic;
        auto& token = s_token;
        auto item = token->enqueue_begin<moodycamel::CanAlloc>( magic );
        item->hdr.type = QueueType::ZoneBegin;
        item->zoneBegin.time = Profiler::GetTime( item->zoneBegin.cpu );
        item->zoneBegin.thread = thread;
        item->zoneBegin.srcloc = (uint64_t)srcloc;
        token->enqueue_finish( magic );
    }

    tracy_force_inline ~ScopedZone()
    {
        Magic magic;
        auto& token = s_token;
        auto item = token->enqueue_begin<moodycamel::CanAlloc>( magic );
        item->hdr.type = QueueType::ZoneEnd;
        item->zoneEnd.time = Profiler::GetTime( item->zoneEnd.cpu );
        item->zoneEnd.thread = m_thread;
        token->enqueue_finish( magic );
    }

    tracy_force_inline void Text( const char* txt, size_t size )
    {
        Magic magic;
        auto ptr = new char[size+1];
        memcpy( ptr, txt, size );
        ptr[size] = '\0';
        auto& token = s_token;
        auto item = token->enqueue_begin<moodycamel::CanAlloc>( magic );
        item->hdr.type = QueueType::ZoneText;
        item->zoneText.thread = m_thread;
        item->zoneText.text = (uint64_t)ptr;
        token->enqueue_finish( magic );
    }

    tracy_force_inline void Name( const char* name )
    {
        Magic magic;
        auto& token = s_token;
        auto item = token->enqueue_begin<moodycamel::CanAlloc>( magic );
        item->hdr.type = QueueType::ZoneName;
        item->zoneName.thread = m_thread;
        item->zoneName.name = (uint64_t)name;
        token->enqueue_finish( magic );
    }

private:
    uint64_t m_thread;
};

}

#endif
