using Avalonia.Controls;
using Avalonia.Data.Converters;
using Avalonia.Media;
using System.Globalization;

namespace PilotLight.Avalonia.Controls;

public partial class ChatMessageView : UserControl
{
    public ChatMessageView()
    {
        InitializeComponent();
    }
}

public sealed class BoolToBrushConverter : IValueConverter
{
    public static readonly BoolToBrushConverter Instance = new();
    private static readonly SolidColorBrush UserBrush = new(Color.Parse("#243B69"));
    private static readonly SolidColorBrush AssistantBrush = new(Color.Parse("#1A2130"));

    public object Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
    {
        var isUser = value is bool b && b;
        return isUser ? UserBrush : AssistantBrush;
    }

    public object? ConvertBack(object? value, Type targetType, object? parameter, CultureInfo culture) => null;
}
