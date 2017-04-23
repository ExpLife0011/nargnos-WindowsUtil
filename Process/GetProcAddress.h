#pragma once
#include "Common.h"

namespace Process
{
	namespace Overwrite
	{
		// NOTICE: ����ĺ���ֻ֧������������, ��ʹ������
		// ���ֲ���(����������)
		_Ret_maybenull_ void* GetProcAddress(_In_ _Notnull_ HMODULE module,
			_In_ _Notnull_ LPCSTR lpProcName);
		// ˳�����
		// function�����Ƕ������ַ�
		_Ret_maybenull_ void* GetProcAddress(_In_ _Notnull_ HMODULE module,
			const _STD function<bool(LPCSTR)>& comp);

		// ���ֲ���, �Զ��ƱȽϺ���
		// @lpProcName: ��ʾ����ָ�룬�����Ǽ���ʲô�ģ���Ҫ�ڻص����Լ�����
		// @comp: function��һ����Ϊ������ڵ㺯�������ڶ�����ΪlpProcName�����ش�С��ϵ,С����<0,��>0,��==0
		_Ret_maybenull_ void* GetProcAddress(_In_ _Notnull_ HMODULE module,
			_In_ _Notnull_ const void* lpProcName, const _STD function<int(LPCSTR, const void*)>& comp);

	}  // namespace Overwrite
}  // namespace Process
