#include "Logger.h"

/**
 * std::ostream overloads
 */
template<class T>
std::ostream & operator<<(std::ostream & os, const std::vector<T> & vec) {
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
            os << vec.at(i);
            if (i < vec.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

template<class K, class V>
std::ostream & operator<<(std::ostream & os, const std::map<K, V> & map) {
    os << "{";
    for (auto it = map.begin(); it != map.end(); it++) {
        os << it->first << ": " << it->second;
        if (it != std::prev(map.end())) {
            os << ", ";
        }
    }
    os << "}";
    return os;
}

std::ostream & operator<<(std::ostream & os, Color color) {
#if defined(WIN)
    static const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    const auto colorNum = static_cast<uint8_t>(color);
    SetConsoleTextAttribute(handle, colorNum);
#elif defined(UNIX)
    os << "\x1b[" << Logger::unixColors.at(color) << "m";
#endif
    return os;
}

template<class ...Args>
const Logger & Logger::debug(Args && ...args) const {
    return log(LogLevel::Debug, args...);
}

template<class ...Args>
const Logger & Logger::info(Args && ...args) const {
    return log(LogLevel::Info, args...);
}

template<class ...Args>
const Logger & Logger::warn(Args && ...args) const {
    return log(LogLevel::Warn, args...);
}

template<class ...Args>
const Logger & Logger::error(Args && ...args) const {
    return log(LogLevel::Error, args...);
}

template<class ...Args>
const Logger & Logger::dev(Args && ...args) const {
    return log(LogLevel::Dev, args...);
}

template<class Arg, class ...Args>
void Logger::devPanic(Arg && first, Args && ...other) {
    std::cout << levelColors.at(LogLevel::Error);
    std::cout << "[DEV PANIC]: " << std::forward<Arg>(first);
    ((std::cout << ' ' << std::forward<Args>(other)), ...);
    std::cout << Color::Reset;
    Logger::nl();

    throw common::Error("Stop after dev panic!");
}

template<class Arg, class... Args>
void Logger::devDebug(Arg && first, Args && ... other) {
    std::cout << levelColors.at(LogLevel::Dev) << "[DEV]: " << Color::Reset << std::forward<Arg>(first);
    ((std::cout << ' ' << std::forward<Args>(other)), ...);
    Logger::nl();
}

template<class Arg, class ...Args>
const Logger & Logger::raw(Arg && first, Args && ...other) const {
    std::cout << std::forward<Arg>(first);
    ((std::cout << ' ' << std::forward<Args>(other)), ...);
    return *this;
}

template<class Arg, class ...Args>
void Logger::colorized(Color color, Arg && first, Args && ...other) {
    std::cout << "\x1b[" <<unixColors.at(color) << 'm'; // Not putting it to `raw`, because it will be prefixed with white-space
    raw(first, other..., Color::Reset).nl();
}

template<class Arg, class ...Args>
void Logger::print(Arg && first, Args && ...other)  {
    std::cout << std::forward<Arg>(first);
    ((std::cout << ' ' << std::forward<Args>(other)), ...);
}

template<class Arg, class ...Args>
std::string Logger::format(Arg && first, Args && ...other) {
    std::stringstream ss;
    ss << first;
    ((ss << ' ' << other), ...);
    return ss.str();
}

template<class Arg, class ...Args>
const Logger & Logger::log(LogLevel level, Arg && first, Args && ...other) const {
    if (not common::Config::getInstance().checkDev()
    and static_cast<uint8_t>(level) < static_cast<uint8_t>(config.level)) {
        return *this;
    }

    const auto & dev = level == LogLevel::Dev;

    if (config.printLevel or dev) {
        if (config.colorize or dev) {
            std::cout << levelColors.at(level);
        }
        std::cout << levelNames.at(level) << ": ";
        if (config.colorize or dev) {
            std::cout << Color::Reset;
        }
    }

    if (config.printOwner or dev) {
        std::cout << "(" << owner << ") ";
    }

    std::cout << std::forward<Arg>(first);

    ((std::cout << ' ' << std::forward<Args>(other)), ...);

    nl();

    return *this;
}
