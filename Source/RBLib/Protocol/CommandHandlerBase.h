///////////////////////////////////////////////////////////////////////////////
//	����� ������ �������� ����������� ������ � ����������� �� ���������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMANDHANDLERBASE_H
#define COMMANDHANDLERBASE_H

#include "..\Communication\InterfaceBase.h"
#include "..\Utils\Functor.h"
#include "..\NullType.h"

namespace Rblib
{
namespace Protocol
{
	/// ��� �������� ����������� ������.
	typedef BasicFunctor<MakeTypeList<CmdInfo *, Communication::InterfaceBase *>::Result> ExternalCommandHandlerFunctor;

	/// \brief ������� ����� �������� ����������� ������. 
	/// \details �� ����� ������ ���� ���������� �������������� � ������� ���� � �����������, 
	/// ����� ����� ���� ������������ �������.
	/// \tparam T ��� �������� �����������-����������.
	template<class T>
	class ExternalCommandHandler
	{
	public:
		ExternalCommandHandler(T *object) : handler(object)
		{
		}
		T *handler;
		void operator ()(CmdInfo *cmd, Communication::InterfaceBase *outputInterface)
		{
			// ���� ����� ���������� � �������� �����������
			// ������ ���� ��������
			handler->CommandHandler(cmd, outputInterface);
		}
	};

	/// \brief ���������� �������� ����������� ������������� ����������� ������.
	/// \details ��������� � ����� ����������� ���������� �� ���������� ��� ��������� ������.
	/// ����� ����������� ��������� ������ �� ���������, � ���������� ��������� � �������� ����������������� �����������.
	/// ��������� �������� ������� ������ �� ����� ������, 
	/// � ���������� ���� ������� ������������ ������������� CommandHandlerBase.
	template<class NotUsedType>
	class _CommandHandlerBase
	{
	protected:

		/// ������������ ������� ����������
		static ExternalCommandHandlerFunctor _externalCommandHandler;

	public:

		/// ��������� ������� ���������� ������
		/// \param commandHandler ������ �� ������� ���������� ������.
		/// \memberof Rblib::Protocol::CommandHandlerBase
		static void SetExternalCommandHandler(const ExternalCommandHandlerFunctor &commandHandler)
		{
			_externalCommandHandler = commandHandler;
		}

		/// ��������� �������, �������� �����.
		/// \details ��������� ��������� �� ��������� � �������� ������� ���������������� ����������.
		/// \param cmd ��������� �� ��������� ���������� �������.
		/// \param outputInterface ��������� ��� ������� ������.
		static inline void ProcessCommand(CmdInfo *cmd, Communication::InterfaceBase *outputInterface)
		{
			if (0 == cmd)
				return;
			switch(cmd->Cmd)
			{
			case CmdDevicePing:
				if (outputInterface)
				{
					CmdInfo answer(*cmd);
					answer.Cmd = CmdDevicePong;
					outputInterface->SendCommand(&answer);
				}
			break;
			// ������� �� ���������� �������� ���������
			case CmdUnknown:
			{
				// ��������� "������� �� ��������������"
				CmdInfo answer(*cmd);
				outputInterface->SendCommand(&answer);
			}
			break;
			// ������� ���������� ��������, ���������� � �� ������������
			default:
				// �������� �������� �����������
				_externalCommandHandler(cmd, outputInterface);
				// ���� ������� �� ���� ���������� ������� ������������
				if (!cmd->Handled)
				{
					// TODO ������� "�� ��������������"
				}
			break;
			}
		}

		// ��������� �������
		static inline void ResendCommand(unsigned int cmd, unsigned char *data)
		{
		}
	};

	template <class NotUsedType>
	ExternalCommandHandlerFunctor _CommandHandlerBase<NotUsedType>::_externalCommandHandler;

	/// \class Rblib::Protocol::CommandHandlerBase
	/// \brief ���������� ����������, ������� �����.
	/// \details ����� �������� ������������ �������������� ������ _CommandHandlerBase, � ������� ���������� ��� ����������������.
	/// \n �������� ��� <tt>typedef _CommandHandlerBase<NullType> %CommandHandlerBase;</tt>
	typedef _CommandHandlerBase<NullType> CommandHandlerBase;

}
}

#endif
