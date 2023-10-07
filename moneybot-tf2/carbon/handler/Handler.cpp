#include "Handler.h"

std::shared_ptr<carbon::element::io_page> Handler::RegisterPage(std::string pageName, const char32_t* whatever) {
	if (mapPage.count(pageName) > 0)
		return mapPage[pageName].element; // return cached

	// make new page
	auto page = std::make_shared<carbon::element::io_page>(pageName, whatever);
	auto sheet = std::make_shared<carbon::element::io_sub_sheet>();


	std::map<std::string, TabAndGroups> contents;
	contents = std::map<std::string, TabAndGroups>();

	mapPage[pageName] = PageAndContents{ page, contents, sheet };

	// add sheet to page
	page->add_child(sheet);

	// add to tabsheet
	tabSheet->add_child(page);

	return page;
}

std::shared_ptr<carbon::element::io_page> Handler::RegisterSubPage(std::string parent, std::string pageName) {
	if (mapPage[parent].contents.count(pageName) > 0)
		return mapPage[parent].contents[pageName].element; // return cached

	// make new subpage
	auto subPage = std::make_shared<carbon::element::io_page>(pageName);

	// cache in the many maps
	std::map<std::string, std::shared_ptr<carbon::element::io_groupbox>> groups;
	groups = std::map<std::string, std::shared_ptr<carbon::element::io_groupbox>>();

	mapPage[parent].contents[pageName] = TabAndGroups{ subPage, groups };

	// add to the tab sheet 
	mapPage[parent].sheet->add_child(subPage);

	return subPage;
}

std::shared_ptr<carbon::element::io_groupbox> Handler::RegisterGroupBox(std::string tab, std::string page, std::string group, carbon::io_vec4 pos=carbon::io_vec4{ 0, 0, 0, 0 }) {
	if (mapPage[tab].contents[page].groups.count(group) > 0)
		return mapPage[tab].contents[page].groups[group];

	auto elgroup = std::make_shared<carbon::element::io_groupbox>(group, group, pos);
	mapPage[tab].contents[page].groups[group] = elgroup;

	mapPage[tab].contents[page].element->add_child(elgroup);

	return elgroup;
}

void Handler::Init(std::shared_ptr<carbon::element::io_tab_sheet> sheet) {
	mapPage = std::map<std::string, PageAndContents>();
	tabSheet = sheet;
}

//void Handler::TabSheetInit(std::shared_ptr<carbon::element::io_tab_sheet> sheet) {
//	for (int i = 0; i < qPage.size(); i++)
//		sheet->add_child(qPage[i]);
//}