#pragma once

#include "asm-impl/coroutine.h"
#include <type_traits>
#include <functional>
#include <exception>
#include <cassert>

struct CoroutineExplicitTerminate {};

struct CoroutineNotEndedException : public std::runtime_error
{
    CoroutineNotEndedException() : std::runtime_error("Trying to destroy a coroutine that is not ended!") {}
};

template <typename OT, typename IT>
class Coroutine final
{
    coroutine_t inCoro;
    enum class CoroState : uint8_t { InHost, InCoroutine, Ended, ForciblyEnded } state;
    std::exception_ptr storedEx;
    
public:
    using OutType = std::remove_reference_t<OT>;
    using InType = std::remove_reference_t<IT>;
    using Handle = std::function<OutType(Coroutine&,InType)>;
    
    struct InInfo
    {
        void* param;
        bool terminate;
    };
    
    Coroutine() : inCoro(null_coro), storedEx(), state(CoroState::InHost) {}
    ~Coroutine() 
    {
        if (!is_coroutine_ended(inCoro)) terminate();
        coroutine_destroy(inCoro); 
    }
    
    // disable copying
    Coroutine(const Coroutine& other) = delete;
    Coroutine& operator=(const Coroutine& other) = delete;
    
    // enable moving
    Coroutine(Coroutine&& other) : inCoro(other.inCoro), state(other.state)
    {
        assert(state != CoroState::InCoroutine);
        other.inCoro = null_coro;
    }
    Coroutine& operator=(Coroutine&& other)
    {
        assert(state != CoroState::InCoroutine);
        inCoro = other.inCoro;
        other.inCoro = null_coro;
        return *this;
    }
    
    // coroutine
    Coroutine(Handle handle, size_t stackSize = 4096) : Coroutine()
    {
        struct PassedIn { Coroutine<OT,IT>& coro; Handle handle; }
        data { *this, handle };
        
        inCoro = coroutine_new([] (coroutine_t, const void* param) -> const void*
        {
            PassedIn* data = (PassedIn*)param;
            Coroutine<OT,IT>& coro = data->coro;
            Handle handle = data->handle;
            
            InInfo *in = (InInfo*)coroutine_yield(coro.inCoro, nullptr);
            if (in->terminate)
            {
                coro.state = CoroState::ForciblyEnded;
                return nullptr;
            }
            
            try
            {
                OutType out = handle(coro, *(InType*)in->param);
                coro.state = CoroState::Ended;
                coroutine_yield(coro.inCoro, (const void*)&out);
            }
            catch (CoroutineExplicitTerminate)
            {
                coro.state = CoroState::ForciblyEnded;
                coro.storedEx = std::current_exception();
            }
            catch (...)
            {
                coro.storedEx = std::current_exception();
            }
            
            return nullptr;
        }, stackSize);
        
        // This first resume is to pass the required info
        coroutine_resume(inCoro, (const void*)&data);
    }
    
    // yield and resume functions
    InType yield(OutType param)
    {
        assert(state == CoroState::InCoroutine);
        state = CoroState::InHost;
        
        const InInfo* info = (const InInfo*)coroutine_yield(inCoro, (const void*)&param);
        if (info->terminate) throw CoroutineExplicitTerminate{};
        else return *(InType*)info->param;
    }
    
    OutType resume(InType param)
    {
        assert(state == CoroState::InHost);
        state = CoroState::InCoroutine;
        InInfo info { &param, false };
        auto result = coroutine_resume(inCoro, (const void*)&info);
        
        if (state == CoroState::Ended)
        {
            auto res = *(OutType*)result;
            coroutine_resume(inCoro, nullptr);
            return res;
        }
        else if (storedEx) std::rethrow_exception(storedEx);
        else return *(OutType*)result;
    }
    
    void terminate()
    {
        assert(state == CoroState::InHost);
        InInfo info { nullptr, true };
        coroutine_resume(inCoro, (const void*)&info);
        
        if (state != CoroState::ForciblyEnded)
            std::rethrow_exception(storedEx);
    }
    
    bool isEnded() const
    {
        return is_coroutine_ended(inCoro);
    }
};
