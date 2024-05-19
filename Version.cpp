#include "pch.h"
#include "Version.h"

using namespace KappaBar;

constexpr char *mon[12] =
{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

// Amount of days in each month
constexpr char mond[12] =
{ 31,    28,    31,    30,    31,    30,    31,    31,    30,    31,    30,    31    };

// constexpr-friendly re-impl of isdigit
constexpr bool c_isdigit(char c)
{
	return (c >= '0') && (c <= '9');
}

// constexpr-friendly re-impl of atoi
constexpr int c_atoi(const char *str)
{
	int ret = 0;
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (!c_isdigit(str[i])) return ret;
		ret = ret * 10 + str[i] - '0';
	}
	return ret;
}

// constexpr-friendly re-impl of strnicmp
constexpr int c_strnicmp(const char *s1, const char *s2, int n)
{
	while (n-- > 0)
	{
		int c1 = *s1++;
		int c2 = *s2++;

		if (c1 != c2)
		{
			if (c1 >= 'a' && c1 <= 'z')
				c1 -= ('a' - 'A');
			if (c2 >= 'a' && c2 <= 'z')
				c2 -= ('a' - 'A');
			if (c1 != c2)
				return c1 < c2 ? -1 : 1;
		}
		if (c1 == '\0')
			return 0;
	}
	return 0;
}

constexpr int CalculateBuildNumber(const char *date, const char *epoch = nullptr)
{
	int m = 0;
	int d = 0;
	int y = 0;

	for (m = 0; m < 11; m++)
	{
		if (0 == c_strnicmp(date, mon[m], 3))
		{
			break;
		}
		d += mond[m];
	}

	d += c_atoi(date + 4) - 1;
	y = c_atoi(date + 7) - 1900;

	int buildNum = d + (int)((y - 1) * 365.25);

	if (((y % 4) == 0) && m > 1)
	{
		buildNum += 1;
	}

	if (epoch)
	{
		buildNum -= CalculateBuildNumber(epoch);
	}

	return buildNum;
}



constexpr char *version = "0.1";
constexpr char *date = __DATE__;
constexpr char *dateFull = __TIME__ " " __DATE__;
constexpr char *epoch = "Jan 08 2024";
constexpr int buildNumber = CalculateBuildNumber(date, epoch);

int Util::GetBuildNumber()
{
	return buildNumber;
}

int Util::GetBuildDate(LPWSTR lpString, int cchMax)
{
	return mbstowcs_s(nullptr, lpString, cchMax, date, cchMax);
}

int Util::GetBuildDateFull(LPWSTR lpString, int cchMax)
{
	return mbstowcs_s(nullptr, lpString, cchMax, dateFull, cchMax);
}

int Util::GetVersion(LPWSTR lpString, int cchMax)
{
	return mbstowcs_s(nullptr, lpString, cchMax, version, cchMax);
}