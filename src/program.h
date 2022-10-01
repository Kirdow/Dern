#pragma once

#include "code.h"
#include "storedvalue.h"

namespace Dern
{
	enum class ECount
	{
		Pre, None, Post
	};

	class Registry;
	class FuncDef;
	class FuncCall;
	class TokenData;
	class Token;
	class StackFrame;
	class Debug;
	class LSystem;
	class LProgram
	{
	public:
		LProgram(const std::string& filePath);
		~LProgram();

		void Run();

		const Ref<LSystem>& GetSystem() const;
	private:
		Ref<StoredValue> RunImpl();
	private:
		int GetIndex(ECount increment)
		{
			int result = m_InstructionIndex;
			switch (increment)
			{
			case ECount::Pre: result = ++m_InstructionIndex; break;
			case ECount::Post: result = m_InstructionIndex++; break;
			case ECount::None: result = m_InstructionIndex; break;
			}
			return result;
		}

		int GetIndex() const { return m_InstructionIndex; }
		void SetIndex(int index) { m_InstructionIndex = index; }

		const Ref<Token>& GetToken(ECount increment = ECount::None, int offset = 0);

		friend class Debug;
		friend class LSystem;
	private:
		Scope<Debug> m_Debug;
		Scope<Code> m_Code;
		Scope<TokenData> m_Data;
		int m_InstructionIndex;
		Ref<LSystem> m_System;
	};
}
