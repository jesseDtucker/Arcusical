#include "CancellationToken.hpp"

void Util::CancellationToken::Cancel() { m_isCanceled = true; }

bool Util::CancellationToken::IsCanceled() { return m_isCanceled; }
