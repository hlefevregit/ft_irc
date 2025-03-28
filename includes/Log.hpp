/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 16:48:01 by ldalmass          #+#    #+#             */
/*   Updated: 2025/03/28 17:07:31 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_HPP
# define LOG_HPP

# include <iostream>
# include <string>
# include <sstream>

# define IN			"┌─ IN  "
# define WALL		"│  "
# define OUT		"└─ OUT "
# define TRAIL		" ───────┄┄╌┈"

# define DEPTH static short 0;

class Log
{
	private:
		static int	depth;
		std::string	functionName;

	public:
		Log(const std::string &funcName);
		~Log();
		void log(const std::string &message);
};



// Macro for automatic logging
# define AUTO_LOG		Log logInstance(__FUNCTION__);

#define LOG(message) { \
    std::ostringstream oss; \
    oss << message; \
    logInstance.log(oss.str()); \
};

#endif