#pragma once

#include "pch.h"

namespace KappaBar
{
namespace Util
{
	int GetBuildNumber();
	int GetBuildDate(LPWSTR lpString, int cchMax);
	int GetBuildDateFull(LPWSTR lpString, int cchMax);
	int GetVersion(LPWSTR lpString, int cchMax);
} // namespace KappaBar
} // namespace Util
