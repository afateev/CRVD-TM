///////////////////////////////////////////////////////////////////////////////
//	������ �������
//	�������� FirTwig2
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PARSER_FIRTWIG2_H
#define PARSER_FIRTWIG2_H

#include "PacketFormat.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{

	///
	/// \brief ����� ������� ������� � ������� ��������� FirTwig2
	///
	class Parser
	{
	public:

		/// ������������ ������� ����������� ������ � ������
		static const unsigned char MaxNestingLevel = 3;

		///
		/// \brief ��������� ��� ������������� ������ ���������� � ������,
		/// ����� �������� ����������� ������ �������
		///
		struct PacketSummaryInfo
		{
			/// ��������� �� �����, ���������� �����
			unsigned char* PacketBuffer;
			/// ������ ������
			unsigned int PacketSize;
			/// ������� ����������� ������ � ������ (1, 2 ��� 3)
			unsigned char NestingLevel;

			/// ���� ������ 0-��, 1-�� � 2-�� ������ �����������
			unsigned char Cmd[MaxNestingLevel];
			/// ������ ������ 0-��, 1-�� � 2-�� ������ �����������
			unsigned char* Data[MaxNestingLevel];
			/// ������ ������ ������ 0-��, 1-�� � 2-�� ������ �����������
			unsigned int DataSize[MaxNestingLevel];

			/// �������� ������
			unsigned char Source;
			/// ���������� ������
			unsigned char Dest;

			/// ��� �������������
			unsigned char Ack;
			/// ���� �������
			bool Request;

			/// ������� ����� �����������
			signed char NetAddress;

			PacketSummaryInfo()
			: PacketBuffer(0)
			, PacketSize(0)
			, NestingLevel(0)
			, Source(0)
			, Dest(0)
			, Ack(0)
			, Request(0)
			, NetAddress(-1)
			{
			}

			/// ��������� ���������� �������� �� ���������� ������ �������� ���������� ����� ���� (UCMD_DATA)
			bool IsModemData() const
			{
				return (NestingLevel >= 1 && Cmd[0] == ModemCmdData && Source == AddrModem && Request == 0);
			}
		};

	protected:

		/// ��������� �� �����
		unsigned char *_data;
		/// ����� ������
		unsigned int _length;

		/// ������� ������� ������
		DevCommandSet _commandSet;

	public:

		/// ����������� ��-���������
		Parser()
		{
			_commandSet = DevCommandSetDefault;
			_data = 0;
			_length = 0;
		}

		/// ���������� ������� ����� ��� ��������
		/*! 
			\param data ��������� �� �����, ���������� �����
			\param length ����� ������
		*/
		void SelectPacket(unsigned char *data, unsigned int length)
		{
			_data = data;
			_length = length;
		}

		/// ������ ������� ������� ������
		/*! 
			\param commandSet ������� ������
		*/
		void SelectCommandSet(DevCommandSet commandSet)
		{
			_commandSet = commandSet;
		}

		// ������� ��������� "����������" ������

		/*!
			\brief �������� ����� ����������
			\return ����� ����������
		*/
		unsigned char GetDstAddress(void)
		{
			HeaderField* head = (HeaderField*)_data;
			return head->Dest;
		}

		/*!
			\brief �������� ����� ���������
			\return ����� ���������
		*/
		unsigned char GetSrcAddress(void)
		{
			HeaderField* head = (HeaderField*)_data;
			return head->Source;
		}

		/*!
			\brief �������� ��� �������
			\return ��� �������
		*/
		unsigned int GetCommand(void)
		{
			HeaderField* head = (HeaderField*)_data;
			return head->Cmd;
		}

		/*!
			\brief ���������� ����� ��������� ������
			\return ����� ��������� ������
		*/
		unsigned int GetDataSize(void)
		{
			HeaderField* head = (HeaderField*)_data;
			return head->Len;
		}

		/*!
			\brief ���������� ��������� �� ��������� ������
			\return ��������� � ����� ������
		*/
		unsigned char * GetData()
		{
			if (GetDataSize() == 0)
				return 0;
			return &(_data[4]);
		}

		/*!
			\brief ���������� ��� �������������
			\return ��� �������������
		*/
		unsigned char GetAckCode(void)
		{
			HeaderField* head = (HeaderField*)_data;
			return head->Ack;
		}

		/*!
			\brief ������ � ����� ������� (Requet)
			\return ���� �������
		*/
		unsigned char GetRequestFlag(void)
		{
			HeaderField* head = (HeaderField*)_data;
			return head->Request;
		}

		/*!
			\brief ���������� ��������� �� ����� ������
			\return ��������� �� ����� ������
		*/
		unsigned char *GetPacketData()
		{
			return _data;
		}
		
		/*!
			\brief ���������� ������ �������� ������
			\return ������ �������� ������
		*/
		unsigned int GetPacketSize()
		{
			return _length;
		}

		/*!
			\brief �������� ������� ����� ����������� ������
			\return ������� ����� ����������� ������
		*/
		char GetNetAddr()
		{
			if (IsModemData())
			{
				return _data[_netAddrInd];
			}
			else
				return -1;
		}

		// ����� �� ���� ����� ������???
		bool GetRequestWise()
		{
			if (IsModemData())
			{
				return ((SubCommandField*)&_data[_subDataModemInd])->Request;
			}
			else
			{
				return ((HeaderField*)_data)->Request;
			}
		}

		// ����� �� ���� ����� ������???
		unsigned char GetAckWise()
		{
			if (IsModemData())
			{
				return ((SubCommandField*)&_data[_subDataModemInd])->Ack;
			}
			else
			{
				return ((HeaderField*)_data)->Ack;
			}
		}

		/*!
			\brief ������� ������
			\param[out] summaryInfo ���������� ������ ������� (������ ���������� � ������)
		*/
		void GetSummaryInfo(PacketSummaryInfo& summaryInfo)
		{
			summaryInfo.Source = ((HeaderField*)_data)->Source;
			summaryInfo.Dest = ((HeaderField*)_data)->Dest;
			summaryInfo.PacketBuffer = _data;
			summaryInfo.PacketSize = _length;

			State state;
			while(state.CurrentLevel < MaxNestingLevel)
			{
				bool stop = false;
				if (StateParserNextCommand(state, stop))
				{
					summaryInfo.Cmd[state.CurrentLevel] = state.CurrentCmd;
					summaryInfo.Data[state.CurrentLevel] = state.CurrentCmdSubData;
					summaryInfo.DataSize[state.CurrentLevel] = state.CurrentCmdSubDataLen;
					summaryInfo.NestingLevel++;
					state.CurrentLevel++;
					if (stop)
					{
						break;
					}
				}
				else
				{
					break;
				}
			}

			GetAddlPacketData(summaryInfo.NetAddress, summaryInfo.Request, summaryInfo.Ack);
		}

	protected:

		/// ��������� �� ��������� ������ ��� ������� �������
		static const unsigned char _subDataStdInd = sizeof(HeaderField);
		/// ��������� �� ��������� ������ ��� �������, ���������� ����� ���� (UCMD_DATA)
		static const unsigned char _subDataModemInd = sizeof(HeaderField) + sizeof(ModemCmdDataHeader); // UCMD_DATA
		/// ���� � ������� ����-����������� � �������, ���������� ����� ���� (UCMD_DATA)
		static const unsigned char _netAddrInd = sizeof(HeaderField) + 2;

	protected:

		/// ���������� ��������� �������
		struct State
		{
			/// ������� ������� � ������
			unsigned int CurrentCmd;

			/// �������� ������ ������� �������
			unsigned char* CurrentCmdSubData;
			/// ����� ��������� ������ ������� �������
			unsigned int CurrentCmdSubDataLen;

			/// ������� ����������� ������� �������
			unsigned char CurrentLevel;

			/// ����������� �� ���������
			State()
			: CurrentCmd(0)
			, CurrentCmdSubData(0)
			, CurrentCmdSubDataLen(0)
			, CurrentLevel(0)
			{
			}
		};

	protected:

		/// �������� �� �������� �� ����� �������� UCMD_DATA
		inline bool IsModemData()
		{
			return (IsModemPacket() && IsModemDataCmdCode());
		}

		// ��������� �� �������� �� ����� ������� �� ������
		inline bool IsModemPacket()
		{
			return (((HeaderField*)_data)->Source == AddrModem);
		}

		// ����� �� ��� ������� ���� UCMD_DATA
		inline bool IsModemDataCmdCode()
		{
			return (((HeaderField*)_data)->Cmd == ModemCmdData && ((HeaderField*)_data)->Request == 0);
		}

		/// \brief ��������� �� ������ ������� ����� ����������� � ���� request � ack
		/// \param[out] netAddr ������� ����� �����������
		/// \param[out] request true - � ������ ���������� ���� �������, false - �� ����������
		/// \param[out] ack true - � ������ ���������� ���� �������, false - �� ����������
		void GetAddlPacketData(signed char& netAddr, bool& request, unsigned char& ack)
		{
			if (IsModemData())
			{
				netAddr = _data[_netAddrInd];
				request = ((SubCommandField*)&_data[_subDataModemInd])->Request;
				ack = ((SubCommandField*)&_data[_subDataModemInd])->Ack;
			}
			else
			{
				netAddr = -1;
				request = ((HeaderField*)_data)->Request;
				ack = ((HeaderField*)_data)->Ack;
			}
		}

		//
		// ������ ������, ���������� ������ ���������� ������ �����������
		//

		/// \brief ������� ��������� ������� (�������� �������)
		/// \param[in,out] state ������� ���������
		/// \param[in,out] stop - ���� ���� ������� �������, �� ����� ������������
		/// \return @c true - � ������ ������� ��������� �������, @c false - ������� �� �������
		bool StateParserNextCommand(State& state, bool &stop)
		{
			HeaderField* head = (HeaderField*)_data;
			
			// �������� �� ����������� ������� �����������?
			if (state.CurrentLevel >= MaxNestingLevel)
				return false;
			
			// ���������� ������� �������� ������
			if (state.CurrentLevel == 0)
			{
				state.CurrentCmd = head->Cmd;
				if (head->Len)
				{
					if (IsModemData())
					{
						state.CurrentCmdSubData = &_data[_subDataModemInd];
						state.CurrentCmdSubDataLen = head->Len - sizeof(ModemCmdDataHeader);
					}
					else
					{
						state.CurrentCmdSubData = &_data[_subDataStdInd];
						state.CurrentCmdSubDataLen = head->Len;
					}
				}
				else
				{
					state.CurrentCmdSubData = 0;
					state.CurrentCmdSubDataLen = 0;
				}
				return true;
			}
			
			// ���������� ������� 1-�� ������, ��������� � UCMD_DATA � UCMD_SYS
			if (IsModemPacket())
			{
				if (state.CurrentLevel == 1)
					return StateParserNextCommandByCommandSetModem(state);
				else
					if (!IsModemDataCmdCode())
						return false;
			}
			
			// ���������� ������ 1-�� � 2-�� ������� �����������, �� ������ �������� Command Set
			bool commandFound = false;
			switch(_commandSet)
			{
			case DevCommandSetRsu:
				commandFound =  StateParserNextCommandByCommandSetRsu(state, stop);
				break;
			case DevCommandSetRsIr:
				commandFound =  StateParserNextCommandByCommandSetRsIr(state, stop);
				break;
			case DevCommandSetRslTransmitter:
				commandFound =  StateParserNextCommandByCommandSetRslTransmitter(state, stop);
				break;
			case DevCommandSetRsTv:
				commandFound =  StateParserNextCommandByCommandSetRsTv(state, stop);
				break;
			}

			if (!commandFound)
				return StateParserNextCommandByCommandSetDefault(state, stop);
			
			return true;
		}

		/// \brief �������� ���� ���� ������� ������� � ��������� ��������� ������
		/// \param[in,out] state ������� ���������
		inline bool StateParserUpdateCmdParamsFullCmd(State& state)
		{
			if (state.CurrentCmdSubDataLen == 0)
				return false; // ��� ��������� ������/���������
			
			state.CurrentCmd = state.CurrentCmdSubData[0];
			
			StateParserUpdateCmdBuffersDefault(state);
			return true;
		}

		/// \brief �������� ��������� �� ��������� ������
		/// \param[in,out] state ������� ���������
		inline void StateParserUpdateCmdBuffersDefault(State& state)
		{
			if (state.CurrentCmdSubDataLen > 1) // ���������� ����� ��������� ������
			{
				state.CurrentCmdSubData = &state.CurrentCmdSubData[1];
				state.CurrentCmdSubDataLen = state.CurrentCmdSubDataLen - 1;
			}
			else // � ���������� ��� ��������� ������
			{
				state.CurrentCmdSubData = 0;
				state.CurrentCmdSubDataLen = 0;
			}
		}

		/// \brief ������� ��������� ������� �� ������ ������� ������ DevCommandSetDefault
		/// \param[in,out] state ������� ���������
		/// \return @c true - ������� �������, @c false - ������� �� �������
		/// \param[in,out] stop - ���� ���� ������� �������, �� ����� ������������
		bool StateParserNextCommandByCommandSetDefault(State& state, bool &stop)
		{
			// �������, ������� �����������
			switch(state.CurrentCmd)
			{
			case DevCmdSystemControl:
			case DevCmdExtended:
				{
					stop = true; // ���������� ����� ��������� � ��������� ������, ������ ������� �� ����
					return StateParserUpdateCmdParamsFullCmd(state);
				}
			}
			return false;
		}

		/// \brief ������� ��������� ������� �� ������ ������� ������ DevCommandSetRsu
		/// \param[in,out] state ������� ���������
		/// \param[in,out] stop - ���� ���� ������� �������, �� ����� ������������
		/// \return @c true - ������� �������, @c false - ������� �� �������
		bool StateParserNextCommandByCommandSetRsu(State& state, bool &stop)
		{
			switch(state.CurrentCmd)
			{
			case DevCmdRsuSeismicInveroment:
			case DevCmdRsuSeismicParamEx:
			case DevCmdRsuExtDevParam:
			case DevCmdRsuExtDevControl:
			case DevCmdRsuSelfTest:
				{
					stop = true; // ���������� ����� ��������� � ��������� ������, ������ ������� �� ����
					return StateParserUpdateCmdParamsFullCmd(state);
				}
			}
			return false;
		}

		/// \brief ������� ��������� ������� �� ������ ������� ������ DevCommandSetRsIr
		/// \param[in,out] state ������� ���������
		/// \param[in,out] stop - ���� ���� ������� �������, �� ����� ������������
		/// \return @c true - ������� �������, @c false - ������� �� �������
		bool StateParserNextCommandByCommandSetRsIr(State& state, bool &stop)
		{
			switch(state.CurrentCmd)
			{
//			case ...:
//				{
//					stop = true; // ���������� ����� ��������� � ��������� ������, ������ ������� �� ����
//					return StateParserUpdateCmdParamsFullCmd(state);
//				}
			}
			return false;
		}

		/// \brief ������� ��������� ������� �� ������ ������� ������ DevCommandSetRslTransmitter
		/// \param[in,out] state ������� ���������
		/// \param[in,out] stop - ���� ���� ������� �������, �� ����� ������������
		/// \return @c true - ������� �������, @c false - ������� �� �������
		bool StateParserNextCommandByCommandSetRslTransmitter(State& state, bool &stop)
		{
			switch(state.CurrentCmd)
			{
			case RslTrasmitterCmdTransmitter:
				{
					stop = true; // ���������� ����� ��������� � ��������� ������, ������ ������� �� ����
					return StateParserUpdateCmdParamsFullCmd(state);
				}
			}
			return false;
		}

		/// \brief ������� ��������� ������� �� ������ ������� ������ DevCommandSetRsTv
		/// \param[in,out] state ������� ���������
		/// \param[in,out] stop - ���� ���� ������� �������, �� ����� ������������
		/// \return @c true - ������� �������, @c false - ������� �� �������
		bool StateParserNextCommandByCommandSetRsTv(State& state, bool &stop)
		{
			switch(state.CurrentCmd)
			{
			case DevCmdRsTvWorkControl:
			case DevCmdRsTvCameraControl:
			case DevCmdRsTvVrControl:
				{
					stop = true; // ���������� ����� ��������� � ��������� ������, ������ ������� �� ����
					return StateParserUpdateCmdParamsFullCmd(state);
				}
			}
			return false;
		}

		/// \brief ������� ��������� ������� �� ������ ������� ������ DevCommandSetModem
		/// \param[in,out] state ������� ���������
		/// \return @c true - ������� �������, @c false - ������� �� �������
		bool StateParserNextCommandByCommandSetModem(State& state)
		{
			switch(state.CurrentCmd)
			{
			case ModemCmdData:
			case ModemCmdSys:
            case DevCmdRsuSeismicInveroment:
				return StateParserUpdateCmdParamsFullCmd(state);
			}
			return false;
		}

	};

}
}
}

#endif
