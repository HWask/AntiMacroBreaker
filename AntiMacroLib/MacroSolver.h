#pragma once
#include <cv.h>
#include <highgui.h>

namespace AntiMacroLib {
	class MacroSolver {
	private:
		enum Type {
			CAPTCHA_TWO_PICS,
			CAPTCHA_THREE_PICS
		};

		const cv::Size WindowSize = cv::Size(221, 203);
		const cv::Point WindowPos = cv::Point(289, 198);

		const int CandY = 139;
		const int MatchesY = 44;

		const cv::Size CandSize = cv::Size(34, 57);

		const std::vector<cv::Point> CandPos = std::vector<cv::Point>{ cv::Point(6, CandY), cv::Point(41, CandY), cv::Point(76, CandY),
			cv::Point(111, CandY), cv::Point(146, CandY), cv::Point(181, CandY) };

		const std::vector<cv::Point> MatchesPos3 = std::vector<cv::Point>{ cv::Point(57, MatchesY), cv::Point(92, MatchesY), cv::Point(127, MatchesY) };
		const std::vector<cv::Point> MatchesPos2 = std::vector<cv::Point>{ cv::Point(74, MatchesY), cv::Point(109, MatchesY) };

		std::vector<cv::Mat> ExtractCandidates(cv::Mat captcha);
		std::vector<cv::Mat> ExtractMatches(cv::Mat captcha);
		std::pair<int, double> matchPicture(cv::Mat matchImg, std::vector<cv::Mat>& Candidates, int ignore1, int ignore2);
		std::vector<cv::Point> calculatePositions(std::vector<int> indices, cv::Rect bb);
		cv::Mat CropMacroWindow(cv::Mat img, cv::Rect bb);
		std::vector<int> MatchPictures(std::vector<cv::Mat> matches, std::vector<cv::Mat> candidates);
		void checkType(cv::Mat img);

		std::vector<cv::Point> SolveMacro(cv::Mat image, cv::Rect bb);
		bool IsMacroPopped(cv::Mat image, cv::Rect& bb);

		Type CaptchaType;
		cv::Mat CaptchaImage;
	public:
		MacroSolver(std::vector<char> data);
		MacroSolver(cv::Mat image);

		bool IsMacroPopped();
		std::vector<cv::Point> Solve();
		cv::Mat GetImage();
	};
}