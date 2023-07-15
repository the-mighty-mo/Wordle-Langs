/*
 * Wordle program
 * Author: Benjamin Hall
 */

#include "players/players.hpp"
#include "players/database.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <numeric>
#include <sstream>
#include <vector>

namespace players {

PlayerInfo::PlayerInfo(std::string username,
                    std::unordered_set<std::string> wordsPlayed,
                    std::array<uint32_t, MAX_NUM_GUESSES> numGuesses,
                    uint32_t maxWinStreak,
                    uint32_t curWinStreak) :
    m_username{std::move(username)},
    m_wordsPlayed{std::move(wordsPlayed)},
    m_numGuesses{std::move(numGuesses)},
    m_maxWinStreak{maxWinStreak},
    m_curWinStreak{curWinStreak}
{}

std::string const *PlayerInfo::GetRandomWord(std::unordered_set<std::string> const &dictionary) const
{
    size_t const unplayedWords = dictionary.size() - m_wordsPlayed.size();
    size_t const randomWordIdx = rand() % unplayedWords;

    auto iter = dictionary.begin();
    for (size_t i = 0; iter != dictionary.end(); ++i) {
        if (i > 0) {
            ++iter;
        }
        while (iter != dictionary.end() && m_wordsPlayed.find(*iter) != m_wordsPlayed.end()) {
            ++iter;
        }

        if (i == randomWordIdx) {
            break;
        }
    }

    if (iter != dictionary.end()) {
        return &*iter;
    } else {
        return nullptr;
    }
}

void PlayerInfo::AddWonWord(std::string word, uint32_t numGuesses)
{
    m_wordsPlayed.insert(std::move(word));
    ++m_numGuesses[numGuesses - 1];
    ++m_curWinStreak;
    m_maxWinStreak = std::max(m_maxWinStreak, m_curWinStreak);
}

void PlayerInfo::AddLostWord(std::string word)
{
    m_wordsPlayed.insert(std::move(word));
    m_curWinStreak = 0;
}

std::string PlayerInfo::ToString() const
{
    std::stringstream playerStr;
    playerStr << "Username: " << m_username << "\n";

    playerStr << "Words Played: ";
    {
        bool firstElem = true;
        for (auto const &wordPlayed : m_wordsPlayed) {
            if (firstElem) {
                firstElem = false;
            } else {
                playerStr << ",";
            }
            playerStr << wordPlayed;
        }
        playerStr << "\n";
    }

    playerStr << "Number of Guesses: ";
    for (size_t i = 0; i < m_numGuesses.size(); ++i) {
        if (i != 0) {
            playerStr << ",";
        }
        playerStr << m_numGuesses[i];
    }
    playerStr << "\n";

    playerStr << "Maximum Win Streak: " << m_maxWinStreak << "\n";
    playerStr << "Current Win Streak: " << m_curWinStreak << "\n";

    return playerStr.str();
}

std::string PlayerInfo::GetStats() const
{
    std::stringstream playerStats;

    playerStats << "Number of Words Played: " << m_wordsPlayed.size() << "\n";

    playerStats << "Win Rate: " << [&] {
            uint32_t const wonGames = std::accumulate(m_numGuesses.begin(), m_numGuesses.end(), 0, std::plus<uint32_t>());
            return (uint32_t)std::round(100.0 * wonGames / m_wordsPlayed.size());
        }() << "\n";

    playerStats << "Current Win Streak: " << m_curWinStreak << "\n";
    playerStats << "Maximum Win Streak: " << m_maxWinStreak << "\n";
    playerStats << "Guess Distribution:";

    double const barFactor = [&] {
        auto const maxNumGuesses = std::max_element(m_numGuesses.begin(), m_numGuesses.end());
        if (maxNumGuesses == m_numGuesses.end() || *maxNumGuesses == 0) {
            return 0.0;
        } else {
            return 12.0 / *maxNumGuesses;
        }
    }();

    for (size_t i = 0; i < m_numGuesses.size(); ++i) {
        size_t const numBars = (size_t)std::round(barFactor * m_numGuesses[i]);
        playerStats << "\n" << (i + 1) << ": " << std::string(numBars, '=') << " " << m_numGuesses[i];
    }

    return playerStats.str();
}

int PlayerInfo::WriteToFile(std::string const &filename) const
{
    std::ofstream file{filename};
    if (!file.is_open()) {
        return -1;
    }

    file << ToString();
    file.close();

    return 0;
}

/* static */ std::optional<PlayerInfo> PlayerInfo::CreateFromFile(std::string const &filename)
{
    std::ifstream file{filename};
    if (!file.is_open()) {
        return std::nullopt;
    }

    static auto const getErrStr = [&]() { return "Error: corrupt player database file: " + filename; };

    std::string line;

    if (!std::getline(file, line)) {
        throw getErrStr();
    }
    auto username = DatabaseEntry<std::string>::FromLine(line, [](auto s) { return std::string{s}; });
    if (!username.has_value()) {
        throw getErrStr();
    }

    if (!std::getline(file, line)) {
        throw getErrStr();
    }
    auto wordsPlayed = DatabaseEntry<std::unordered_set<std::string>>::FromSet(line, [](auto s) { return std::string{s}; });
    if (!wordsPlayed.has_value()) {
        throw getErrStr();
    }

    if (!std::getline(file, line)) {
        throw getErrStr();
    }
    auto numGuessesList = DatabaseEntry<std::vector<uint32_t>>::FromVector(line, [](auto s) { return std::stol(std::string{s}); });
    if (!numGuessesList.has_value()) {
        throw getErrStr();
    }

    if (!std::getline(file, line)) {
        throw getErrStr();
    }
    auto maxWinStreak = DatabaseEntry<uint32_t>::FromLine(line, [](auto s) { return std::stol(std::string{s}); });
    if (!maxWinStreak.has_value()) {
        throw getErrStr();
    }

    if (!std::getline(file, line)) {
        throw getErrStr();
    }
    auto curWinStreak = DatabaseEntry<uint32_t>::FromLine(line, [](auto s) { return std::stol(std::string{s}); });
    if (!curWinStreak.has_value()) {
        throw getErrStr();
    }

    file.close();

    std::array<uint32_t, MAX_NUM_GUESSES> numGuesses{};
    for (size_t i = 0; i < numGuesses.size() && i < numGuessesList->value.size(); ++i) {
        numGuesses[i] = numGuessesList->value[i];
    }

    return std::optional{
            PlayerInfo{std::move(username->value), std::move(wordsPlayed->value),
                    std::move(numGuesses), maxWinStreak->value, curWinStreak->value}
        };
}

}
