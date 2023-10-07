#pragma once

#include <map>
#include <queue>

#include "../carbon/carbon.hpp"

struct TabAndGroups {
	std::shared_ptr<carbon::element::io_page> element;
	std::map<std::string, std::shared_ptr<carbon::element::io_groupbox>> groups;
};

struct PageAndContents {
	std::shared_ptr<carbon::element::io_page> element;
	std::map<std::string, TabAndGroups> contents;
	std::shared_ptr<carbon::element::io_sub_sheet> sheet;
};

namespace Handler {
	std::shared_ptr<carbon::element::io_page> RegisterPage(std::string pageName, const char32_t* whatever);
	std::shared_ptr<carbon::element::io_page> RegisterSubPage(std::string parent, std::string pageName);
	std::shared_ptr<carbon::element::io_groupbox> RegisterGroupBox(std::string tab, std::string page, std::string group, carbon::io_vec4 pos);
	void Init(std::shared_ptr<carbon::element::io_tab_sheet> sheet);
//	static void TabSheetInit(std::shared_ptr<carbon::element::io_tab_sheet>);
	inline std::map<std::string, PageAndContents> mapPage;
	inline std::shared_ptr<carbon::element::io_tab_sheet> tabSheet;
};