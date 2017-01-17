#include "HookApi.h"
#include <cmath>
#include <cassert>
#include <PeImage\PeDecoder.h>
#include <Process\EnvironmentBlock.h>
#include <Process\WriteProcessMemory.h>
#include <Process\VirtualProtect.h>
namespace Hook
{
	// jmp xxx
#define E9_JMP_LEN 5
	// jmp [xxx], ��Ҫ���϶�Ӧ��ַ�ĳ���
#define FF25_JMP_LEN 6

	// ��С���ݳ���
#ifdef _WIN64
	const int BACKUPLEN = FF25_JMP_LEN + sizeof(DWORD64);
#else
	const int BACKUPLEN = FF25_JMP_LEN + sizeof(DWORD);
#endif




	void _EmitE9Jmp(PVOID pos, PVOID des)
	{
		assert(pos != nullptr);
		assert(des != nullptr);

		PBYTE& apiAddr = (PBYTE&)pos;
		apiAddr[0] = 0xe9;
		*(PDWORD)(&apiAddr[1]) = (PBYTE)des - (PBYTE)pos - E9_JMP_LEN;
	}

	void _EmitFF25Jmp(PVOID pos, PVOID des)
	{
		assert(pos != nullptr);
		assert(des != nullptr);

		PBYTE& apiAddr = (PBYTE&)pos;
		apiAddr[0] = 0xff;
		apiAddr[1] = 0x25;
		apiAddr += 2;

		// 32д���Ե�ַ,64д��Ե�ַ, ָ������ָ��Ľ���λ��
#ifdef _WIN64
		*(PDWORD)apiAddr = (DWORD)0;
#else
		*(PDWORD)apiAddr = (DWORD)(apiAddr + sizeof(DWORD));
#endif
		apiAddr += sizeof(DWORD);
		// д��Ŀ���ַ
		*reinterpret_cast<HANDLE_PTR*>(apiAddr) = reinterpret_cast<HANDLE_PTR>(des);

	}

	bool IsFF25Jmp(PVOID addr)
	{
		assert(addr != nullptr);
		return ((PBYTE)addr)[0] == 0xff && ((PBYTE)addr)[1] == 0x25;
	}

	bool IsE9Jmp(PVOID addr)
	{
		assert(addr != nullptr);
		return ((PBYTE)addr)[0] == 0xe9;
	}

	// offsetΪָ�ʼλ��
	// Ϊjmp��Ŀ�����������ָ�릤
	bool _RelocJmp(PBYTE des, PVOID oldAddr, int offset)
	{
		auto jmpPos = des + offset;
		if (IsFF25Jmp(jmpPos))
		{
			// ֻ֧�ֱ�hook
#ifndef _WIN64
			if (*(PDWORD)&jmpPos[2] != 0)
			{
				// �޸ľ���ƫ��,32λ����
				jmpPos += 2;
				*(PDWORD)jmpPos = (DWORD)(jmpPos + sizeof(DWORD));
			}
#endif
			return true;
		}
		return false;
		// TODO: ����������ת���,�������ʹ�ò�ͬ��ʽhook��ʱ����Ҫע��
	}


	//	int GetCodeBackupLen(PVOID api, int minLen)
	//	{
	//#ifdef _WIN64
	//		GetInstructionLen gil(false);
	//#else
	//		GetInstructionLen gil(true);
	//#endif
	//		int len = 0;  // ʵ��ָ��ݳ���
	//		int loopTimes = 0;
	//		// ��ȡ����ͷ���ݳ���
	//		while (len < minLen)
	//		{
	//			if (loopTimes++ > minLen)
	//			{
	//				return 0;
	//			}
	//			len += gil.GetLen((PBYTE)api + len);
	//		}
	//		return len;
	//	}

	//	PVOID HookApi(PVOID api, PVOID hook)
	//	{
	//#ifdef _WIN64
	//		GetInstructionLen gil(false);
	//#else
	//		GetInstructionLen gil(true);
	//#endif
	//		int len = 0;
	//		int tmpLen = 0;  // �ٶ�ÿһ��ָ���1BYTEʱ�ۼƵĳ���
	//		if (IsFF25Jmp(api))
	//		{
	//			len = BACKUPLEN;
	//		}
	//		else
	//		{
	//			// ��ȡ����ͷ���ݳ���
	//			while (len < BACKUPLEN)
	//			{
	//				len += gil.GetLen((PBYTE)api + len);
	//				if (tmpLen++ >= BACKUPLEN)
	//				{
	//					// ��ȡʧ�ܣ�����ȡָ��Ȳ�����BUG������ԭ��
	//					return nullptr;
	//				}
	//			}
	//		}
	//		auto result = _HookApi(api, hook, len);
	//
	//
	//		// �ض�λ����ͷ��ת,��ʵ���ظ�hook(���hook������ܲ���)
	//		if (result)
	//		{
	//			tmpLen = 0;
	//			while (tmpLen < len)
	//			{
	//				// �޸���ת
	//				if (_RelocJmp((PBYTE)result, api, tmpLen))
	//				{
	//					break;
	//				}
	//				tmpLen += gil.GetLen((PBYTE)result + tmpLen);
	//			}
	//		}
	//		return result;
	//	}


	bool IsLongDistance(PVOID addr1, PVOID addr2)
	{
		assert(addr1 != nullptr);
		assert(addr2 != nullptr);
		LONGLONG distance = (PBYTE)addr1 - (PBYTE)addr2;
		return abs(distance) >= 0x7fff0000;
	}


	bool Backup(PVOID api, PVOID hook, PBYTE buffer, int backupLen, int bufferLen)
	{
		using namespace Process;
		assert(bufferLen >= backupLen + BACKUPLEN);
		// ����ԭ����
		memcpy(buffer, api, backupLen);
		DWORD oldProtect = 0;

		// ���ñ��ݵ�ַ������
		if (!Overwrite::VirtualProtect(buffer, bufferLen, PAGE_EXECUTE_READWRITE, &oldProtect))
		{
			return false;
		}
		
		// ���ñ��ݴ�����ת(����,��Ҫ���Ǿ���
		auto jmpBackAddr = (PBYTE)api + backupLen;
		auto jmpCode = buffer + backupLen;
		if (IsLongDistance(jmpCode, jmpBackAddr))
		{
			_EmitFF25Jmp(jmpCode, jmpBackAddr);
		}
		else
		{
			_EmitE9Jmp(jmpCode, jmpBackAddr);
		}

		return true;
	}


	/*
	result -> | api(backupLen) | jmp api+backuplen -> ����������ԭʼ����
	api -> |jmp hook|  -> �����Ҫ����ԭʼ����Ͳ����������ַ
	*/
	bool _HookApi(PVOID api, PVOID hook, int backupLen, PVOID * oldFunc, int minBackupLen, void(*emitFunc)(PVOID,PVOID))
	{
		using namespace Process;
		assert(api != nullptr);
		assert(hook != nullptr);

		if (oldFunc != nullptr)
		{
			assert(backupLen >= minBackupLen);

			*oldFunc = nullptr;
			auto backupBufferLen = backupLen + BACKUPLEN;
			auto backup = new BYTE[backupBufferLen];

			// ����ԭ����
			if (!Backup(api, hook, backup, backupLen, backupBufferLen))
			{
				delete[] backup;
				return false;
			}

			// �������
			*oldFunc = backup;
		}
		else
		{
			backupLen = minBackupLen;
		}

		DWORD oldProtect = 0;
		// ����api����������
		if (!Overwrite::VirtualProtect(api, backupLen, PAGE_EXECUTE_READWRITE, &oldProtect))
		{
			if (oldFunc != nullptr)
			{
				delete[] *oldFunc;
				*oldFunc = nullptr;
			}
			return false;
		}
		// set hook
		emitFunc(api, hook);

		// �ָ�������
		if (!Overwrite::VirtualProtect(api, backupLen, oldProtect, &oldProtect))
		{
			// ����ʧ�ܣ��ָ�����
			if (oldFunc != nullptr)
			{
				memcpy(api, *oldFunc, backupLen);
				delete[] *oldFunc;
				*oldFunc = nullptr;
			}
			return false;
		}
		return true;
	}
	
	
	bool HookApi_FF25(PVOID api, PVOID hook, int backupLen, PVOID * oldFunc)
	{
		return _HookApi(api, hook, backupLen, oldFunc, BACKUPLEN, _EmitFF25Jmp);
	}
	// ʹ��e9 jmp�İ汾,�������Զ������
	bool HookApi_E9(PVOID api, PVOID hook, int backupLen, PVOID * oldFunc)
	{
		assert(!IsLongDistance(api, hook));
		return _HookApi(api, hook, backupLen, oldFunc, E9_JMP_LEN, _EmitE9Jmp);
	}
	//bool HookApi(PVOID api, PVOID hook, int backupLen, PVOID * oldFunc)
	//{
	//	if (IsE9Jmp(api) || IsFF25Jmp(api))
	//	{
	//		return false;
	//	}
	//	if (IsLongDistance(api, hook))
	//	{
	//		return HookApi_FF25(api, hook, backupLen, oldFunc);
	//	}
	//	else
	//	{
	//		return HookApi_E9(api, hook, backupLen, oldFunc);
	//	}
	//}
}  // namespace Hook