#include "cli/progress_bar.hpp"

#include <iostream>
#include <string>
#include <vector>


namespace cpm {

ProgressBar::ProgressBar(const std::string &title, int width, char symbol,
						 int max_value)
	: stages({Stage(title, width, symbol, max_value)}),
	  active_stage(Stage(title, width, symbol, max_value)) {}

void ProgressBar::add_stage(const std::string &title, int width, char symbol,
						 	int max_value) {
	Stage new_stage(title, width, symbol, max_value);
	this->stages.push_back(new_stage);
	this->active_stage = new_stage;
}

void ProgressBar::print_active_stage() const {
	std::cout << this->active_stage.title << "\n";
	std::cout << this->active_stage.bar << this->active_stage.suffix << "\n";
}

void ProgressBar::update_suffix(const std::string &new_suffix) {
	this->active_stage.suffix = new_suffix;
}

void ProgressBar::set_active_bar(const std::string &new_bar) {
	this->active_stage.bar = new_bar;
}

// prefix increment
ProgressBar& ProgressBar::operator++() {
	// actual increment takes place here
	this->active_stage.value++;
	this->fill_bar();
	return *this; // return new value by reference
}

// postfix increment
ProgressBar ProgressBar::operator++(int) {
	ProgressBar old = *this; // copy old value
	operator++();  // prefix increment
	return old;    // return old value
}

// prefix decrement
ProgressBar& ProgressBar::operator--() {
	// actual decrement takes place here
	this->active_stage.value--;
	this->fill_bar();
	return *this; // return new value by reference
}

// postfix decrement
ProgressBar ProgressBar::operator--(int) {
	ProgressBar old = *this; // copy old value
	operator--();  // prefix decrement
	return old;    // return old value
}

void ProgressBar::fill_bar() {
	std::string new_bar = "   [";
	if (this->active_stage.max_value != 0) {
		double step = static_cast<double>(this->active_stage.max_value) /
										  this->active_stage.width;
		for (double i = 0; i < this->active_stage.max_value; i += step) {
			if (i < this->active_stage.value) {
				new_bar += this->active_stage.symbol;
			} else {
				new_bar += " ";
			}
		}

	} else {
		for (int i = 0; i < this->active_stage.width; i++) {
			if (i == this->active_stage.value % this->active_stage.width) {
				new_bar += this->active_stage.symbol;
			} else {
				new_bar += " ";
			}
		}
	}
	new_bar += "]";
	this->active_stage.bar = new_bar;
}

ProgressBar::Stage::Stage(const std::string &title, int width, char symbol,
			  			  int max_value)
	: title(title),
	  bar("   [          ]"),
	  width(width),
	  symbol(symbol),
	  max_value(max_value),
	  value(0) {}

} // namespace cpm
