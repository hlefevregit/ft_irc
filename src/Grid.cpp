/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Grid.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 22:38:13 by ldalmass          #+#    #+#             */
/*   Updated: 2025/03/28 18:22:33 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Grid.hpp"

AnsiToMircConverter::~AnsiToMircConverter()
{
	return;
}

AnsiToMircConverter::AnsiToMircConverter()
{
	_ansiToMirc[30] = 1;   // Black
	_ansiToMirc[31] = 4;   // Red
	_ansiToMirc[32] = 3;   // Green
	_ansiToMirc[33] = 8;   // Yellow
	_ansiToMirc[34] = 2;   // Blue
	_ansiToMirc[35] = 6;   // Purple
	_ansiToMirc[36] = 10;  // Cyan
	_ansiToMirc[37] = 15;  // White
	_ansiToMirc[0] = -1;   // Reset (remove color)
}

int			AnsiToMircConverter::_stringToInt(const std::string &s) const
{
	std::stringstream ss(s);
	int num = 0;
	ss >> num;
	return num;
}

std::string	AnsiToMircConverter::_intToString(int num) const
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

std::string	AnsiToMircConverter::convertAnsiToMirc(const std::string &input) const
{
	std::string					result;
	std::string::const_iterator	it = input.begin();

	while (it != input.end())
	{
		if (*it == '\033' && (it + 1) != input.end() && *(it + 1) == '[')
		{
			std::string::const_iterator	seq_start = it + 2;
			std::string::const_iterator	seq_end = seq_start;

			while (seq_end != input.end() && *seq_end != 'm')
				++seq_end;

			if (seq_end != input.end())
			{
				std::string	codeStr(seq_start, seq_end);
				int			code = _stringToInt(codeStr);

				if (_ansiToMirc.find(code) != _ansiToMirc.end())
				{
					if (_ansiToMirc.at(code) == -1)
						result += "\017"; // Reset mIRC colors
					else
					{
						result += "\003";
						if (_ansiToMirc.at(code) < 10)
							result += "0";
						result += _intToString(_ansiToMirc.at(code));
					}
				}
				it = seq_end + 1; // Move iterator past the ANSI sequence
				continue;
			}
		}
		result += *it;
		++it;
	}
	return result;
}

void	addCellData(t_cell_data &data, std::string message, unsigned int alignment)
{
	data.alignment.push_back(alignment);
	data.message.push_back(message);
	return ;
}

int getTerminalWidth(void)
{
	struct winsize	w;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
		return 80; // Default width if we can't get it
	return (w.ws_col);
}

void	printCeiling(unsigned short width, unsigned short max_cell_size)
{
	for (unsigned short i = 0; i < width; i++)
	{
		// Top left
		if (i == 0)
		{
			std::cout << "┌";
			continue;
		}
		// Top Right
		if (i == width - 2)
		{
			std::cout << "┐";
			continue;
		}
		// Separators
		if (i % max_cell_size == 0 && i != width - 2)
		{
			std::cout << "┬";
			continue;
		}
		// Ceiling
		if (i < width - 2)
			std::cout << "─";
	}
	std::cout << std::endl;
	return ;
}

void	printFloor(unsigned short width, unsigned short max_cell_size)
{
	for (unsigned short i = 0; i < width; i++)
	{
		// Top left
		if (i == 0)
		{
			std::cout << "└";
			continue;
		}
		// Top Right
		if (i == width - 2)
		{
			std::cout << "┘";
			continue;
		}
		// Separators
		if (i % max_cell_size == 0 && i != width - 2)
		{
			std::cout << "┴";
			continue;
		}
		// Ceiling
		if (i < width - 2)
			std::cout << "─";
	}
	std::cout << std::endl;
	return ;
}

void	printYSeparator(unsigned short width, unsigned short max_cell_size)
{
	for (unsigned short i = 0; i < width; i++)
	{
		// Left
		if (i == 0)
		{
			std::cout << "├";
			continue;
		}
		// Right
		if (i == width - 2)
		{
			std::cout << "┤";
			continue;
		}
		// Separators
		if (i % max_cell_size == 0 && i != width - 2)
		{
			std::cout << "┼";
			continue;
		}
		// Line
		if (i < width - 2)
			std::cout << "─";
	}
	std::cout << std::endl;
	return ;
}

unsigned int	getStringLenghtWithoutColors(std::string &str)
{
	// This function count a string's lenght
	// without accounting for color codes
	// such as "\033[0;31m", which is interpreted
	// as a red character colorization,
	// or "\033[0m" which reset any formating.

	unsigned int 			len = 0;
	bool					stop_count = false;
	std::string::iterator	start = str.begin();
	std::string::iterator	end = str.end();

	while (start != end)
	{
		if (start != end && *start == '\033')
		{
			stop_count = true;
			--len;
		}
		if (start != end && *start == 'm' && stop_count == true)
			stop_count = false;
		if (stop_count == false)
			++len;
		++start;
	}

	return (len);
}

std::string	trimWithoutTakingOutColorCodes(std::string input, int max_lenght)
{
	int			 			len = 0;
	bool					stop_add = false;
	std::string				result;
	std::string::iterator	start = input.begin();
	std::string::iterator	end = input.end();

	while (start != end)
	{
		if (*start == '\033')
			stop_add = true;
		if (stop_add == false && len < max_lenght - 1)
		{
			result += *start;
			len++;
		}
		else if (stop_add == true)
			result += *start;
		if (*start == 'm' && stop_add == true)
			stop_add = false;
		++start;
	}
	result += '.';

	return (result);
}

std::string	truncate_entry(std::string input, unsigned short max_lenght, unsigned short alignment)
{
	// This function truncate a message to fit
	// in a defined max_lenght, it also
	// provide text alignment such as left,
	// right, and center aligmnent.

	int len = getStringLenghtWithoutColors(input);

	if (len > max_lenght)
		return (trimWithoutTakingOutColorCodes(input, max_lenght));
	if (len == max_lenght)
		return (input);
	else
	{
		int	i = -1;
		std::string	spaces;

		while (++i < (max_lenght - (int)getStringLenghtWithoutColors(input)))
			spaces = spaces.append(" ");

		switch (alignment)
		{
			default:
				return (input + spaces);
			case LEFT:
				return (input + spaces);
			case RIGHT:
				return (spaces + input);
			case CENTER:
				// std::cout << CENTER << std::endl;
				std::string::iterator	start = spaces.begin();
				std::string::iterator	end = spaces.end();
				std::string::iterator	mid = start + (end - start) / 2;
				std::string				spaces_left = std::string(start, mid);
				std::string				spaces_right = std::string(mid, end);
				return (spaces_left + input + spaces_right);
		}
	}
}

void	write_line(std::string str, unsigned short max_col_lenght, int alignment, bool is_last_cell)
{
	// Fill in the gaps and truncate if needed
	std::string	trunc_str = truncate_entry(str, max_col_lenght - 1, alignment);

	// Print the cell
	std::cout << "│";
	std::cout << trunc_str;
	if (is_last_cell == true)
	{
		std::cout << "│";
		std::cout << std::endl;
	}
	return;
}

void	printGrid(t_cell_data &cell_data, unsigned short width, unsigned short x_cells, unsigned short y_cells)
{
	// Ensure width does not exceed terminal width
	unsigned short	col = getTerminalWidth();
	unsigned short	max_cell_size;
	width = (width > col) ? col : width;
	max_cell_size = width - 2;

	// Ensure y_cells is at least 1
	if (y_cells == 0)
		y_cells = 1;

	// Ensure x_cells is at least 1
	if (x_cells == 0)
		x_cells = 1;

	// Adjust x_cells to properly fit within width
	if (x_cells > width - 2)
		x_cells = (width - 2) / 2;  // Ensure at least 2 spaces per cell

	// Calculate max_cell_size safely
	if (x_cells > 1)
	{
		max_cell_size = (width - 2) / x_cells;
		if (max_cell_size == 0)
			max_cell_size = 1;  // Ensure at least 1 character per cell
		width = (max_cell_size * x_cells) + 2;
	}

	// Add a default behaviour to last element in vector
	cell_data.message.push_back("");
	cell_data.alignment.push_back(0);

	// Get iterators for message and alignment vectors
	std::vector<std::string>::iterator	msg_start = cell_data.message.begin();
	std::vector<std::string>::iterator	msg_end = cell_data.message.end();
	std::vector<int>::iterator			ali_start = cell_data.alignment.begin();
	std::vector<int>::iterator			ali_end = cell_data.alignment.end();

	// Print Grid
	printCeiling(width, max_cell_size);
	for (unsigned short y = 0; y < y_cells; y++)
	{
		// Print cells
		for (unsigned short i = 0; i < x_cells; i++)
		{
			// Ensure we do not go past the last element
			if (msg_start == msg_end)
				--msg_start;
			if (ali_start == ali_end)
				--ali_start;

			// Print the cell
			bool is_last = (i == x_cells - 1);
			write_line(*msg_start, max_cell_size, *ali_start, is_last);

			// Move to the next element if possible
			if (msg_start != msg_end)
				++msg_start;
			if (ali_start != ali_end)
				++ali_start;
		}

		if (y != y_cells - 1)
			printYSeparator(width, max_cell_size);
	}
	printFloor(width, max_cell_size);
}
