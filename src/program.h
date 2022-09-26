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
	class LProgram
	{
	public:
		LProgram(const std::string& filePath);
		~LProgram();

		void Run();
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
	private:
		Scope<Code> m_Code;
		Scope<TokenData> m_Data;
		int m_InstructionIndex;
		Ref<Token> m_TokenStack[32];
		Ref<Registry> m_Registry;
		std::unordered_map<std::string, Ref<FuncDef>> m_FuncMap;
		std::vector<Ref<StackFrame>> m_FrameStack;
	};
}
