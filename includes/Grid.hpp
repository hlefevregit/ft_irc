#ifndef GRID_HPP
# define GRID_HPP

# define RESET		"\033[0m"
# define RED		"\033[0;31m"
# define GREEN		"\033[0;32m"
# define YELLOW		"\033[0;33m"
# define BLUE		"\033[0;34m"
# define MAGENTA	"\033[0;35m"
# define CYAN		"\033[0;36m"

# define INFO		"\033[0;34m[INFO]\033[0m  "
# define DEBUG		"\033[0;33m[DEBUG]\033[0m "
# define ERROR		"\033[0;31m[ERROR]\033[0m "

# include <string>
# include <vector>
# include <map>
# include <iostream>
# include <sstream>

// get terminal's column count
# include <unistd.h>
# include <sys/ioctl.h>

#endif

typedef enum	s_alignment
{
	LEFT,
	RIGHT,
	CENTER
}				t_alignment;

typedef struct	s_cell_data
{
	std::vector<std::string>	message;
	std::vector<int>			alignment;
}				t_cell_data;

class	AnsiToMircConverter
{
	private:
		~AnsiToMircConverter();

		std::map<int, int>	_ansiToMirc;
		std::string			_intToString(int num) const;
		int					_stringToInt(const std::string &s) const;

	public:
		AnsiToMircConverter();

		std::string	convertAnsiToMirc(const std::string &input) const;
};

int 			getTerminalWidth(void);
void			addCellData(t_cell_data &data, std::string message, unsigned int alignment);
void			printCeiling(unsigned short width, unsigned short max_cell_size);
void			printFloor(unsigned short width, unsigned short max_cell_size);
void			printYSeparator(unsigned short width, unsigned short max_cell_size);
void			write_line(std::string str, unsigned short max_col_lenght, int alignment, bool is_last_cell);
void			printTable(t_cell_data &cell_data, unsigned short width, unsigned short x_cells, unsigned short y_cells);
std::string		trimWithoutTakingOutColorCodes(std::string input, int max_lenght);
std::string		truncate_entry(std::string input, unsigned short max_lenght, unsigned short alignment);
unsigned int	getStringLenghtWithoutColors(std::string &str);