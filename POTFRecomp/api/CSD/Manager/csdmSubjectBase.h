#pragma once

#include "Marathon.inl"

namespace Chao::CSD
{
    template<typename TObserver, typename TObservee>
    class SubjectBase
    {
    public:
        struct Vftable
        {
            be<uint32_t> m_fpDtor;
            be<uint32_t> m_fpGetObservee;
        };

        xpointer<Vftable> m_pVftable;
        MARATHON_INSERT_PADDING(0x0C);

        ~SubjectBase();
        TObservee* GetObservee() const;
    };
}

#include "CSD/Manager/csdmSubjectBase.inl"
