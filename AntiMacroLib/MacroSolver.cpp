#include "MacroSolver.h"


std::vector<cv::Mat> AntiMacroLib::MacroSolver::ExtractCandidates(cv::Mat captcha) {
	std::vector<cv::Mat> Candidates;

	for (auto pos : CandPos) {
		cv::Mat candidate;
		cv::Point2f center(pos.x + CandSize.width / 2.0, pos.y + CandSize.height / 2.0);
		getRectSubPix(captcha, CandSize, center, candidate);
		Candidates.push_back(candidate);
	}

	return Candidates;
}

std::vector<cv::Mat> AntiMacroLib::MacroSolver::ExtractMatches(cv::Mat captcha) {
	std::vector<cv::Mat> Matches;

	if (CaptchaType == Type::CAPTCHA_THREE_PICS) {
		for (auto pos : MatchesPos3) {
			cv::Mat candidate;
			cv::Point2f center(pos.x + CandSize.width / 2.0, pos.y + CandSize.height / 2.0);
			getRectSubPix(captcha, CandSize, center, candidate);
			Matches.push_back(candidate);
		}
	}

	if (CaptchaType == Type::CAPTCHA_TWO_PICS) {
		for (auto pos : MatchesPos2) {
			cv::Mat candidate;
			cv::Point2f center(pos.x + CandSize.width / 2.0, pos.y + CandSize.height / 2.0);
			getRectSubPix(captcha, CandSize, center, candidate);
			Matches.push_back(candidate);
		}
	}

	return Matches;
}

std::pair<int, double> AntiMacroLib::MacroSolver::matchPicture(cv::Mat matchImg, std::vector<cv::Mat>& Candidates, int ignore1, int ignore2) {
	double minVal = std::numeric_limits<double>::max();
	int ret = 0;

	for (int i = 0; i < Candidates.size(); i++) {
		if (i == ignore1 || i == ignore2)
			continue;

		cv::Mat Result;
		Result.create(1, 1, CV_32FC1);
		matchTemplate(Candidates[i], matchImg, Result, cv::TM_SQDIFF);

		double min;
		minMaxLoc(Result, &min, nullptr, nullptr, nullptr);

		if (min < minVal) {
			minVal = min;
			ret = i;
		}
	}

	return std::pair<int, double>(ret, minVal);
}

std::vector<cv::Point> AntiMacroLib::MacroSolver::calculatePositions(std::vector<int> indices, cv::Rect bb) {
	const int y = 180;
	std::vector<cv::Point> points;

	for (auto i : indices) {
		int x = 6 + 35 * i + 17;
		points.push_back(cv::Point(bb.x + x, bb.y + y));
	}

	return points;
}

bool AntiMacroLib::MacroSolver::IsMacroPopped(cv::Mat img, cv::Rect& bb) {
	if (img.at<cv::Vec3b>(209, 500) == cv::Vec3b(0, 0, 255) && //red dot
		img.at<cv::Vec3b>(210, 444) == cv::Vec3b(0, 255, 0) && 
		img.at<cv::Vec3b>(210, 359) == cv::Vec3b(0, 255, 0) &&
		img.at<cv::Vec3b>(315, 448) == cv::Vec3b(0, 255, 0) && 
		img.at<cv::Vec3b>(315, 339) == cv::Vec3b(0, 255, 0)) {

		bb.x = WindowPos.x;
		bb.y = WindowPos.y;
		bb.width = WindowSize.width;
		bb.height = WindowSize.height;
		return true;
	}

	if (img.at<cv::Vec3b>(209, 500) == cv::Vec3b(0, 255, 255) && //yellow dot
		img.at<cv::Vec3b>(210, 444) == cv::Vec3b(0, 255, 0) &&
		img.at<cv::Vec3b>(210, 359) == cv::Vec3b(0, 255, 0) &&
		img.at<cv::Vec3b>(315, 448) == cv::Vec3b(0, 255, 0) &&
		img.at<cv::Vec3b>(315, 339) == cv::Vec3b(0, 255, 0) ) {

		bb.x = WindowPos.x;
		bb.y = WindowPos.y;
		bb.width = WindowSize.width;
		bb.height = WindowSize.height;
		return true;
	}

	return false;
}

void AntiMacroLib::MacroSolver::checkType(cv::Mat img) {
	cv::Mat gray;
	cv::cvtColor(img, gray, CV_BGR2GRAY);

	cv::Mat threshold;
	cv::threshold(gray, threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

	cv::Mat patch;
	cv::getRectSubPix(threshold, cv::Size(114, 65), cv::Point2f(109, 73), patch);

	int count = 0;

	for (int i = 58; i <= 60; i++) {
		for (int j = 0; j < patch.cols; j++) {
			if (patch.at<uchar>(cv::Point(j, i)) == 255) { //white px
				count++;
			}
		}
	}

	if (count > 240) {
		CaptchaType = Type::CAPTCHA_THREE_PICS;
	}
	else {
		CaptchaType = Type::CAPTCHA_TWO_PICS;
	}
}

cv::Mat AntiMacroLib::MacroSolver::CropMacroWindow(cv::Mat img, cv::Rect bb) {
	cv::Mat captchaImg;
	cv::Point2f center(bb.x + bb.width / 2.0, bb.y + bb.height / 2.0);
	getRectSubPix(img, bb.size(), center, captchaImg);
	
	return captchaImg;
}


std::vector<int> AntiMacroLib::MacroSolver::MatchPictures(std::vector<cv::Mat> matches, std::vector<cv::Mat> candidates) {
	int ign1 = -1;
	int ign2 = -1;
	std::vector<int> indices;

	for (int i = 0; i < matches.size(); i++) {
		auto r = matchPicture(matches[i], candidates, ign1, ign2);
		indices.push_back(r.first);

		if (i == 0)
			ign1 = r.first;

		if (i == 1)
			ign2 = r.first;
	}

	return indices;
}


std::vector<cv::Point> AntiMacroLib::MacroSolver::SolveMacro(cv::Mat image, cv::Rect bb) {
	auto cropped = CropMacroWindow(image, bb);
	checkType(cropped);
	auto cands = ExtractCandidates(cropped);
	auto matches = ExtractMatches(cropped);
	auto indices = MatchPictures(matches, cands);
	auto pos = calculatePositions(indices, bb);

	return pos;
}

std::vector<cv::Point> AntiMacroLib::MacroSolver::Solve() {
	cv::Rect bb;
	if (IsMacroPopped(CaptchaImage, bb)) {
		return SolveMacro(CaptchaImage, bb);
	}
	else {
		return std::vector<cv::Point>();
	}
}

AntiMacroLib::MacroSolver::MacroSolver(std::vector<char> data) {
	cv::Mat im(data);
	CaptchaImage = imdecode(im, 1);
}

bool AntiMacroLib::MacroSolver::IsMacroPopped() {
	cv::Rect bb;
	return IsMacroPopped(CaptchaImage, bb);
}

AntiMacroLib::MacroSolver::MacroSolver(cv::Mat image) {
	CaptchaImage = image;
}

cv::Mat AntiMacroLib::MacroSolver::GetImage() {
	return CaptchaImage;
}