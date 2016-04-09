
#ifndef DRIVERS_H
#define DRIVERS_H

#include "../RbLib/Rblib.h"
#include "../MainPanel/MegaLib/MAX6951.h"
#include "../MainPanel/MegaLib/stm32/f107/Spi.h"
#include "../MainPanel/MegaLib/mmc/mmc.h"

namespace MainPanel
{
	template
		<
			class BoardType
		>
	class Drivers
	{
	public:
		typedef BoardType Board;
		
		typedef Rblib::Driver::Bus6800
			<
				typename Board::DisplayConnection::Port,
				typename Board::DisplayConnection::PortStruct
			> LcdDataBus;
		
		typedef Rblib::Driver::ScreenRA8835< LcdDataBus > LcdScreenType;
		typedef Rblib::ScreenBuffer<LcdScreenType, 320 / 8, 240 * 2> LcdType;
		
		static LcdType Lcd;
		
		typedef Rblib::DrawContext DrawContextType;
		static DrawContextType DrawContext;
		
		typedef Rblib::FbgFont FontType;
		static FontType GeneralFont;
		static FontType MenuBigFont;
		
		typedef Max6951<SpiWrapper<Spi::_C, typename Board::Gpio::C, 7, false, false> > StatorDisplay;
		typedef Max6951<SpiWrapper<Spi::_C, typename Board::Gpio::C, 6, false, false> > RotorDisplay;
		typedef MMC<SpiWrapper<Spi::_C, typename Board::Gpio::D, 0, false, false> > FlashType;
		
		typedef Rblib::Driver::Rs485<typename Board::Rs485Interface> Rs485Type;
		Rs485Type Rs485;
	public:
		static void Init()
		{
			Board::Init();
			
			DrawContext.Init(&Lcd);
			DrawContext.SelectFont(&GeneralFont);
		}
	};
	
	template <class Board>
		Drivers<Board>::LcdType Drivers<Board>::Lcd;
	
	template <class Board>
		Drivers<Board>::DrawContextType Drivers<Board>::DrawContext;
	
	template <class Board>
		Drivers<Board>::FontType Drivers<Board>::GeneralFont(Rblib::GOSTtypeB8x16);
	
	template <class Board>
		Drivers<Board>::FontType Drivers<Board>::MenuBigFont(Rblib::GOSTtypeB16x32);
}

typedef MainPanel::Drivers<MainPanel::Board> Drivers;

#endif