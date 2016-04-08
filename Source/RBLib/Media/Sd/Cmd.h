
#ifndef _MEDIA_SD_CMD_H_
#define _MEDIA_SD_CMD_H_

namespace Rblib
{
namespace Media
{
namespace Sd
{
    class Cmd
    {
    public:

        Cmd()
        {
        }

        Cmd(unsigned char code, unsigned long arg)
        : _code(code)
        , _argument(arg)
        {
        }

        inline unsigned char Code() const
        {
            return _code;
        }

        inline unsigned long Argument() const
        {
            return _argument;
        }

    private:

        unsigned char _code;
        unsigned long _argument;
    };

    //
    // CMD0
    //

    class Cmd0 : public Cmd
    {
    public:

        Cmd0() : Cmd(0, 0)
        {
        }
    };

    //
    // CMD2
    //

    class Cmd2 : public Cmd
    {
    public:

        Cmd2() : Cmd(2, 0)
        {
        }
    };

    //
    // CMD3
    //

    class Cmd3 : public Cmd
    {
    public:

        Cmd3() : Cmd(3, 0)
        {
        }
    };

    class Cmd7 : public Cmd
    {
    public:

        Cmd7(unsigned short rca) : Cmd(7, BuildArgument(rca))
        {
        }

    private:

        inline unsigned long BuildArgument(unsigned short rca)
        {
            return ((unsigned long)rca << 16);
        }
    };

    //
    // CMD8
    //

    class Cmd8 : public Cmd
    {
    public:

        enum Voltage
        {
            VoltageDefault = 1
        };

        Cmd8(unsigned char checkPattern) : Cmd(8, BuildArgument(VoltageDefault, checkPattern))
        {
        }

    private:

        inline unsigned long BuildArgument(unsigned char voltage, unsigned char checkPattern)
        {
            return checkPattern | ((voltage & 0x0F) << 8);
        }
    };

	//
	// CMD9
	//

	class Cmd9 : public Cmd
	{
	public:

		Cmd9() : Cmd(9, BuildArgument(0))
		{
		}

		Cmd9(unsigned short rca) : Cmd(9, BuildArgument(rca))
		{
		}

	private:

		inline unsigned long BuildArgument(unsigned short rca)
		{
			return rca << 16;
		}

	};

	//
	// CMD10
	//

	class Cmd10 : public Cmd
	{
	public:

		Cmd10() : Cmd(10, BuildArgument(0))
		{
		}

		Cmd10(unsigned short rca) : Cmd(10, BuildArgument(rca))
		{
		}

	private:

		inline unsigned long BuildArgument(unsigned short rca)
		{
			return rca << 16;
		}

	};

    //
    // CMD12
    //

    class Cmd12 : public Cmd
    {
    public:

        Cmd12() : Cmd(12, 0)
        {
        }

    };

    //
    // CMD13
    //

    class Cmd13 : public Cmd
    {
    public:

		Cmd13() : Cmd(13, BuildArgument(0))
		{
		}

        Cmd13(unsigned short rca) : Cmd(13, BuildArgument(rca))
        {
        }

    private:

        inline unsigned long BuildArgument(unsigned short rca)
        {
            return rca << 16;
        }

    };

    //
    // CMD16
    //

    class Cmd16 : public Cmd
    {
    public:

        Cmd16(unsigned long blockLength) : Cmd(16, blockLength)
        {
        }

    };

    //
    // CMD17
    //

    class Cmd17 : public Cmd
    {
    public:

        Cmd17(unsigned long address) : Cmd(17, address)
        {
        }

    };

    //
    // CMD24
    //

    class Cmd24 : public Cmd
    {
    public:

        Cmd24(unsigned long address) : Cmd(24, address)
        {
        }

    };

    //
    // CMD55
    //

    class Cmd55 : public Cmd
    {
    public:

        Cmd55(unsigned short rca) : Cmd(55, BuildArgument(rca))
        {
        }

    private:

        inline unsigned long BuildArgument(unsigned short rca)
        {
            return rca << 16;
        }
    };

	//
	// CMD58
	//

	class Cmd58 : public Cmd
	{
	public:

		Cmd58() : Cmd(58, 0)
		{
		}

	private:
	};

	//
	// CMD59
	//

	class Cmd59 : public Cmd
	{
	public:

		Cmd59() : Cmd(59, 0) // аргументы по умолчанию: проверка CRC выключена
		{
		}

		Cmd59(bool crcOn) : Cmd(59, crcOn ? 1 : 0) // CRC выключено
		{
		}

	private:
	};

    // Application commands

    //
    // ACMD6
    //

    class ACmd6 : public Cmd
    {
    public:

        ACmd6(bool enable4BitBus) : Cmd(6, BuildArgument(enable4BitBus))
        {
        }

    private:

        inline unsigned long BuildArgument(bool enable4BitBus)
        {
            return enable4BitBus ? 2 : 0;
        }
    };

    //
    // ACMD41
    //

    class ACmd41 : public Cmd
    {
    public:

        enum Voltage
        {
            VoltageLevel1 = 0, // 2.7-2.8
            VoltageLevel2 = 1, // 2.8-2.9
            VoltageLevel3 = 2, // 2.9-3.0
            VoltageLevel4 = 3, // 3.0-3.1
            VoltageLevel5 = 4, // 3.1-3.2
            VoltageLevel6 = 5, // 3.2-3.3
            VoltageLevel7 = 6, // 3.3-3.4
            VoltageLevel8 = 7, // 3.4-3.5
            VoltageLevel9 = 8, // 3.5-3.6
            VoltageLevelResponse
        };

        ACmd41(unsigned char voltage, bool sdhcSupported, bool powerSaving, bool switchTo1_8)
        : Cmd(41, BuildArgument(voltage, sdhcSupported, powerSaving, switchTo1_8))
        {
        }

        ACmd41(unsigned char voltage)
        : Cmd(41, BuildArgument(voltage, true, false, false))
        {
        }

        ACmd41()
        : Cmd(41, BuildArgument(VoltageLevel3, true, false, false))
        {
        }

    private:

        inline unsigned long BuildArgument(unsigned char voltage, bool sdhcSupported, bool powerSaving, bool switchTo1_8)
        {
            return ((voltage == VoltageLevelResponse) ? 0 : (0x00008000 << voltage)) 
                    | (switchTo1_8 ? 0x01000000 : 0) 
                    | (powerSaving ? 0x10000000 : 0) 
                    | (sdhcSupported ? 0x40000000 : 0);
        }

    };
}
}
}

#endif // _MEDIA_SD_CMD_H_

