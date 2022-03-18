#pragma once

#include <array>
#include <optional>
#include <string>
#include <unordered_set>

namespace players {

/**
 * Contains information about a Wordle player.
 * 
 * A player has a:
 * - username
 * - list of words played
 * - guess distribution
 * - maximum win streak
 * - current win streak
 */
class PlayerInfo {
private:
    static constexpr size_t MAX_NUM_GUESSES = 6;

    std::string m_username;
    std::unordered_set<std::string> m_wordsPlayed;
    std::array<uint32_t, MAX_NUM_GUESSES> m_numGuesses{};
    uint32_t m_maxWinStreak{};
    uint32_t m_curWinStreak{};

public:
    /**
     * Initializes data for a new player.
     * 
     * @param username
     *        The username of the player
     */
    PlayerInfo(std::string username);

    /**
     * Loads data for an existing player.
     * 
     * @param username
     *        The username of the player
     * @param wordsPlayed
     *        A set of words the player has already played
     * @param numGuesses
     *        An array containing the number of times the
     *        player has guessed a word in (i + 1) attempts,
     *        where i is the index of the array from [0, 6)
     * @param maxWinStreak
     *        The maximum winning streak
     * @param curWinStreak
     *        The current winning streak
     */
    PlayerInfo(std::string username,
            std::unordered_set<std::string> wordsPlayed,
            std::array<uint32_t, MAX_NUM_GUESSES> numGuesses,
            uint32_t maxWinStreak,
            uint32_t curWinStreak);

    /**
     * Returns this player's username.
     */
    std::string const &GetUsername() const;
    
    /**
     * Gets a random word the player has not yet played.
     * 
     * @param dictionary
     *        The dictionary from which to find a word
     */
    std::string const &GetRandomWord(std::unordered_set<std::string> const &dictionary) const;

    /**
     * Adds a word the player has successfully guessed to their database.
     * 
     * This function increments the current win streak, adds
     * the guess to the list of words played and to the guess
     * distribution, and updates the max win streak if appropriate.
     * 
     * @param word
     *        The word the player successfully guessed
     * @param num_guesses
     *        The number of guesses the player made
     */
    void AddWonWord(std::string word, uint32_t num_guesses);

    /**
     * Adds a word the player has failed to guess to their database.
     * 
     * This function resets the current win streak to 0 and adds
     * the guess to the list of words played. The number of guesses
     * is not added to the player's guess distribution.
     * 
     * @param word
     *        The word the player failed to guess
     */
    void AddLostWord(std::string word);

    /**
     * Returns a string representation of this player's information.
     */
    std::string ToString() const;

    /**
     * Returns a string with formatted player statistics.
     * 
     * Player statistics consist of:
     * - Number of words played
     * - Win rate
     * - Current and max win streak
     * - Guess distribution
     */
    std::string GetStats() const;

    /**
     * Writes a player's data to a file.
     * 
     * @param filename
     *        The file to which to write
     */
    int WriteToFile(std::string const &filename) const;

    /**
     * Reads a player's information from a file.
     * 
     * The function throws an exception on parse errors.
     * 
     * @param filename
     *        The file from which to read
     * @return
     *        A player's information if the operation
     *        succeeded, or a nullopt
     */
    static std::optional<PlayerInfo> CreateFromFile(std::string const &filename);
};

}
