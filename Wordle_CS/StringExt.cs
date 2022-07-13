namespace Wordle_CS;

public static class StringExt
{
    public static IEnumerable<string> Lines(this string source)
    {
        var linesArr = source.Split('\n');
        var lines = linesArr[^1].Length == 0 ? linesArr.Take(linesArr.Length - 1) : (IEnumerable<string>)linesArr;
        return lines.Select(s => s.TrimEnd('\r'));
    }
}
