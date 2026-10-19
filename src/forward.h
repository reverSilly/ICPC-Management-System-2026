#include<string>
#include<vector>

namespace reverSilly::ICPC
{
	struct Time
	{
		int t;
	};
	struct Problem
	{
		std::string name;

	};
	struct Submission
	{
		Problem *problem;
		enum class Status
		{
			Accepted,
			WrongAnswer,
			RuntimeError,
			TimeLimitExceed
		}status;
		Time time;
	};
	struct Group
	{
		std::string name;
		std::vector<Submission>submissions;
		Time 
	};
}