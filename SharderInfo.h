#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class SharderReader {
public:
	SharderReader(const char* filePath) {
		if (filePath == nullptr) { return; }
		m_path = filePath;
	}
	~SharderReader() {}
	bool load() {
		std::ifstream file(m_path);
		if (!file.is_open()) {
			return false;
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		m_data = buffer.str();
		return true;
	}
	const char* data() {
		return m_data.c_str();
	}
private:
	std::string m_data;
	std::string m_path;
};