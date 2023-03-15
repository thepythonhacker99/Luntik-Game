#pragma once

#include "vec2.h"
#include "Alignment.h"

#include <iostream>

namespace Luntik::Utils {
    class Transform {
    public:
        Transform(vec2 pos={0, 0}, vec2 size={0, 0}, Alignment::Alignment2D align={ Alignment::BACK, Alignment::BACK }) {
            this->m_Pos = pos;
            this->m_Size = size;
            setAlignment(align);
        }

        ~Transform() {

        }

        vec2 getPosWithAlignment() const { return m_PosWithAlignment; }
        vec2 getPosWithAlignment(Alignment::Alignment2D alignment) {
            return vec2(m_Pos - (m_Size * ((vec2)(alignment) / 2)));
        }

        static vec2 getPosWithAlignment(vec2 pos, vec2 size, Alignment::Alignment2D alignment) {
            return vec2(pos - (size * ((vec2)(alignment) / 2)));
        }

        vec2 getMiddle() {
            return m_PosWithAlignment + (m_Size / 2);
        }

        vec2 getRawPos() const { return this->m_Pos; }
        vec2 getRawSize() const { return this->m_Size; }
        Alignment::Alignment2D getAlignment() const { return this->m_Alignment; }

        void setPos(vec2 pos) { this->m_Pos = pos; this->m_PosWithAlignment = getPosWithAlignment(getAlignment()); }
        void setSize(vec2 size) { this->m_Size = size; this->m_PosWithAlignment = getPosWithAlignment(getAlignment()); }
        void setAlignment(Alignment::Alignment2D alignment) { this->m_Alignment = alignment; this->m_PosWithAlignment = getPosWithAlignment(alignment); }

    private:
        Alignment::Alignment2D m_Alignment;

        vec2 m_Pos;
        vec2 m_Size;

        vec2 m_PosWithAlignment;
    };
}