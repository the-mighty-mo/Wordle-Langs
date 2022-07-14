namespace Wordle_CS;

/// <summary>
/// Extension methods for the string class
/// </summary>
public static class StringExt
{
    /// <summary>
    /// Splits a string into its separate lines.
    /// <br/>
    /// Lines are ended with either a newline (<c>\n</c>) or
    /// a carriage return with a line feed (<c>\r\n</c>).
    /// <para/>
    /// The final line ending is optional. A string that
    /// ends with a final line ending will return the same
    /// lines as an otherwise identical string without a
    /// final line ending.
    /// </summary>
    /// <param name="source"></param>
    /// <returns>An iterator over the lines of the string</returns>
    public static IEnumerable<string> Lines(this string str)
    {
        var linesArr = str.Split('\n');
        var lines = linesArr[^1].Length == 0 ? linesArr.Take(linesArr.Length - 1) : (IEnumerable<string>)linesArr;
        return lines.Select(s => s.TrimEnd('\r'));
    }
}
