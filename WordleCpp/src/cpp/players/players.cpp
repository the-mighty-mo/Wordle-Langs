/*
 * Wordle program
 * Author: Benjamin Hall
 */

#include "players/players.h"
#include "players/database.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <numeric>
#include <sstream>
#include <vector>

namespace players {

PlayerInfo::PlayerInfo(std::string username)
{
    m_username = std::move(username);
}

PlayerInfo::PlayerInfo(std::string username,
                    std::unordered_set<std::string> wordsPlayed,
                    std::array<uint32_t, MAX_NUM_GUESSES> numGuesses,
                    uint32_t maxWinStreak,
                    uint32_t curWinStreak)
{
    m_username = std::move(username);
    m_wordsPlayed = std::move(wordsPlayed);
    m_numGuesses = std::move(numGuesses);
    m_maxWinStreak = maxWinStreak;
    m_curWinStreak = curWinStreak;
}

std::string const &PlayerInfo::GetUsername() const
{
    return m_username;
}

std::string PlayerInfo::GetRandomWord(std::unordered_set<std::string> const &dictionary) const
{
    size_t unplayedWords = dictionary.size() - m_wordsPlayed.size();
    size_t randomWordIdx = rand() % unplayedWords;

    auto iter = dictionary.begin();
    for (int i = 0; i < randomWordIdx; ++i) {
        ++iter;
    }
    return *iter;
}

void PlayerInfo::AddWonWord(std::string const &word, uint32_t numGuesses)
{
    m_wordsPlayed.insert(word);
    ++m_numGuesses[numGuesses - 1];
    ++m_curWinStreak;
    m_maxWinStreak = std::max(m_maxWinStreak, m_curWinStreak);
}

void PlayerInfo::AddLostWord(std::string const &word)
{
    m_wordsPlayed.insert(word);
    m_curWinStreak = 0;
}

std::string PlayerInfo::ToString() const
{
    std::stringstream playerStr;
    playerStr << "Username: " << m_username << std::endl;

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
        playerStr << std::endl;
    }

    playerStr << "Number of Guesses: "
        << std::accumulate(m_numGuesses.begin(), m_numGuesses.end(), 0, std::plus<uint32_t>())
        << std::endl;

    playerStr << "Maximum Win Streak: " << m_maxWinStreak << std::endl;
    playerStr << "Current Win Streak: " << m_curWinStreak << std::endl;

    return playerStr.str();
}

std::string PlayerInfo::GetStats() const
{
    std::stringstream playerStats;

    playerStats << "Number of Words Played: " << m_wordsPlayed.size() << std::endl;

    playerStats << "Win Rate: " << [&] {
            int wonGames = std::accumulate(m_numGuesses.begin(), m_numGuesses.end(), 0, std::plus<uint32_t>());
            return (uint32_t)std::round(100.0 * wonGames / m_wordsPlayed.size());
        }() << std::endl;
    
    playerStats << "Current Win Streak: " << m_curWinStreak << std::endl;
    playerStats << "Maximum Win Streak: " << m_maxWinStreak << std::endl;
    playerStats << "Guess Distribution:";

    double barFactor = [&] {
        auto maxNumGuesses = std::max_element(m_numGuesses.begin(), m_numGuesses.end());
        if (maxNumGuesses == m_numGuesses.end() || *maxNumGuesses == 0) {
            return 0.0;
        } else {
            return 12.0 / *maxNumGuesses;
        }
    }();

    for (int i = 0; i < m_numGuesses.size(); ++i) {
        int numBars = (int)std::round(barFactor * m_numGuesses[i]);
        playerStats << std::endl << (i + 1) << ": " << std::string(numBars, '=') << m_numGuesses[i];
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

    static std::string const errStr = "Error: corrupt player database file: " + filename;

    std::string line;

    if (!std::getline(file, line)) {
        throw errStr;
    }
    auto username = DatabaseEntry<std::string>::FromLine(line, [](auto const &s) { return s; });
    if (!username.has_value()) {
        throw errStr;
    }

    if (!std::getline(file, line)) {
        throw errStr;
    }
    auto wordsPlayed = DatabaseEntry<std::string>::FromHashset(line, [](auto const &s) { return s; });
    if (!wordsPlayed.has_value()) {
        throw errStr;
    }

    if (!std::getline(file, line)) {
        throw errStr;
    }
    auto numGuessesList = DatabaseEntry<uint32_t>::FromVector(line, [](auto const &s) { return std::stol(s); });
    if (!numGuessesList.has_value()) {
        throw errStr;
    }

    if (!std::getline(file, line)) {
        throw errStr;
    }
    auto maxWinStreak = DatabaseEntry<uint32_t>::FromLine(line, [](auto const &s) { return std::stol(s); });
    if (!maxWinStreak.has_value()) {
        throw errStr;
    }

    if (!std::getline(file, line)) {
        throw errStr;
    }
    auto curWinStreak = DatabaseEntry<uint32_t>::FromLine(line, [](auto const &s) { return std::stol(s); });
    if (!curWinStreak.has_value()) {
        throw errStr;
    }

    file.close();

    std::array<uint32_t, MAX_NUM_GUESSES> numGuesses{};
    for (int i = 0; i < numGuesses.size() && i < numGuessesList->value.size(); ++i) {
        numGuesses[i] = numGuessesList->value[i];
    }

    return std::optional{
            PlayerInfo{std::move(username->value), std::move(wordsPlayed->value),
                    std::move(numGuesses), maxWinStreak->value, curWinStreak->value}
        };
}

}
