/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Grid.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 22:38:06 by ldalmass          #+#    #+#             */
/*   Updated: 2025/04/07 14:16:19 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

		std::map<int, int>	_ansiToMirc;
		std::string			_intToString(int num) const;
		int					_stringToInt(const std::string &s) const;

	public:
		AnsiToMircConverter();
		~AnsiToMircConverter();

		std::string	convertAnsiToMirc(const std::string &input) const;
};

int 			getTerminalWidth(void);
void			addCellData(t_cell_data &data, std::string message, unsigned int alignment);
void			printCeiling(unsigned short width, unsigned short max_cell_size);
void			printFloor(unsigned short width, unsigned short max_cell_size);
void			printYSeparator(unsigned short width, unsigned short max_cell_size);
void			write_line(std::string str, unsigned short max_col_lenght, int alignment, bool is_last_cell);
void			printGrid(t_cell_data &cell_data, unsigned short width, unsigned short x_cells, unsigned short y_cells);
std::string		trimWithoutTakingOutColorCodes(std::string input, int max_lenght);
std::string		truncate_entry(std::string input, unsigned short max_lenght, unsigned short alignment);
unsigned int	getStringLenghtWithoutColors(std::string &str);

std::vector<std::string>	getGrid(t_cell_data &cell_data, unsigned short width, unsigned short x_cells, unsigned short y_cells);
std::string					getCeiling(unsigned short width, unsigned short max_cell_size);
std::string					getFloor(unsigned short width, unsigned short max_cell_size);
std::string					getYSeparator(unsigned short width, unsigned short max_cell_size);
std::string					getLine(std::string str, unsigned short max_col_lenght, int alignment, bool is_last_cell);