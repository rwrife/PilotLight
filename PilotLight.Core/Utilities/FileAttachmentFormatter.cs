using System.Text;
using PilotLight.Core.Models;

namespace PilotLight.Core.Utilities;

public static class FileAttachmentFormatter
{
    public static async Task<string> BuildAttachmentContextAsync(IReadOnlyCollection<Attachment> attachments, CancellationToken cancellationToken)
    {
        if (attachments.Count == 0)
        {
            return string.Empty;
        }

        var builder = new StringBuilder();
        builder.AppendLine("Attached files:");

        foreach (var attachment in attachments)
        {
            cancellationToken.ThrowIfCancellationRequested();
            builder.Append("- ").AppendLine(attachment.DisplayName);

            if (!File.Exists(attachment.FilePath))
            {
                builder.AppendLine("  [file missing]");
                continue;
            }

            try
            {
                var extension = Path.GetExtension(attachment.FilePath).ToLowerInvariant();
                var isText = extension is ".txt" or ".md" or ".json" or ".cs" or ".xml" or ".yaml" or ".yml" or ".log";
                if (!isText)
                {
                    builder.AppendLine("  [binary or unsupported preview type]");
                    continue;
                }

                var content = await File.ReadAllTextAsync(attachment.FilePath, cancellationToken).ConfigureAwait(false);
                var preview = content.Length > 4000 ? content[..4000] + "\n...[truncated]" : content;
                builder.AppendLine("  --- begin preview ---");
                builder.AppendLine(preview);
                builder.AppendLine("  --- end preview ---");
            }
            catch
            {
                builder.AppendLine("  [unable to preview file]");
            }
        }

        return builder.ToString();
    }
}
