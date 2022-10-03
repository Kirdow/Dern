#include "parsetoken.h"

namespace Dern
{
    void LogicalToken::Clear()
    {
        for (auto l : Left)
        {
            if (l->IsType(PTokenType::Logic))
            {
                const_cast<LogicalToken*>(l->Cast<LogicalToken>())->Clear();
            }
        }

        for (auto r : Right)
        {
            if (r->IsType(PTokenType::Logic))
            {
                const_cast<LogicalToken*>(r->Cast<LogicalToken>())->Clear();
            }
        }

        Left.clear();
        Right.clear();
    }
    
}