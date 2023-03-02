#ifndef PROGRESS_H
#define PROGRESS_H

#include <string>
#include <vector>


namespace cpm {

/**
 * @brief A class representing a cpm progress bar
 */
class ProgressBar {

	public:

	/**
	 * @brief Constructor for progress bar. This creates the first stage of the
	 *		  new progress bar and sets it as the active stage.
	 * 
	 * @param title the title of the first stage
	 * @param max_value the maximum value of the first stage progress bar. By
	 * 		  default it is 0, which means that the maximum value is unknown. In
	 * 		  that case the progress bar circles through infinitely to simulate
	 * 		  progress.
	 */
	ProgressBar(const std::string &title, int max_value = 0);

	/**
	 * @brief Add a new stage to the current progress bar
	 * 
	 * @param title the title of the new stage
	 * @param max_value the maximum value of the new stage progress bar. By
	 * 		  default it is 0, which means that the maximum value is unknown. In
	 * 		  that case the progress bar circles through infinitely to simulate
	 * 		  progress.
	 */
	void add_stage(const std::string &title, int max_value = 0);

	/**
	 * @brief Print the active progress bar stage
	 */
	void print_active_stage() const;

	/**
	 * @brief Update/Add custom message after the active progress bar
	 * 
	 * @param new_suffix the new message
	 */
	void update_suffix(const std::string &new_suffix);

	/**
	 * @brief Set the entire active progress bar to a new one
	 * 
	 * @param new_suffix the new progress bar
	 */
	void set_active_bar(const std::string &new_bar);

	// prefix increment
	ProgressBar& operator++();

	// postfix increment
	ProgressBar operator++(int);

	// prefix decrement
	ProgressBar& operator--();

	// postfix decrement
	ProgressBar operator--(int);


	private:

	/**
	 * @brief A class representing one of the progress bar's stages
	 */
	class Stage {

		public:

		/**
		 * @brief Constructor for progress bar stage
		 * 
		 * @param title the title of the new stage
		 * @param max_value the maximum value of the new stage progress bar. By
		 * 		  default it is 0, which means that the maximum value is
		 * 		  unknown. In that case the progress bar circles through
		 * 		  infinitely to simulate progress.
		 */
		Stage(const std::string &title, int max_value = 0);

		std::string title;
		std::string bar;
		std::string suffix;	
		// int width;
		// char symbol;
		int max_value;
		int value;

		private:
	};

	std::vector<Stage> stages;
	Stage active_stage;

	/**
	 * @brief Fill the current active progress bar up to the value specified by
	 * 		  this->active_stage.value (or circle through to simulate progress).
	 */
	void fill_bar();
};

} // namespace cpm


#endif	// PROGRESS_H
