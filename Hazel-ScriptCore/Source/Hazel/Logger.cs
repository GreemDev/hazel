namespace Hazel
{
	public class Logger
	{
		public static void Trace(string message) => InternalCalls.Logger_Trace(message);
		public static void Info(string message) => InternalCalls.Logger_Info(message);
		public static void Warn(string message) => InternalCalls.Logger_Warn(message);
		public static void Error(string message) => InternalCalls.Logger_Error(message);
		public static void Critical(string message) => InternalCalls.Logger_Critical(message);
#pragma warning disable IDE1006 // Naming Styles
		public static void cout(string message) => InternalCalls.Logger_cout(message);
#pragma warning restore IDE1006 // Naming Styles
	}
}
