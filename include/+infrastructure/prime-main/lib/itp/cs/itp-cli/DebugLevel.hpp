#pragma once

#include <cstdint>

namespace itp {

	public ref class DebugLevel {
    public:

        /**
         * \brief \c <b>0x00</b> Отсутствует.
         */
        static const uint8_t NONE = 0x00;

        /**
         * \brief \c <b>0x01</b> FATAL.
         */
        static const uint8_t FATAL = 0x01;

        /**
         * \brief \c <b>0x02</b> ERROR.
         */
        static const uint8_t ERROR = 0x02;

        /**
         * \brief \c <b>0x03</b> WARNING.
         */
        static const uint8_t WARNING = 0x03;

        /**
         * \brief \c <b>0x04</b> INFO.
         */
        static const uint8_t INFO = 0x04;

        /**
         * \brief \c <b>0x05</b> DEBUG.
         */
        static const uint8_t DEBUG = 0x05;

        /**
         * \brief \c <b>0x06</b> TRACE.
         */
        static const uint8_t TRACE = 0x06;
    };

}
