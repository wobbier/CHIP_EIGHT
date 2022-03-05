using System.IO;
using Sharpmake;

public class SDL
{
    private static readonly string kEASTLPath = "[project.SharpmakeCsPath]/ThirdParty/SDL";

    public static void ConfigureAll(Project.Configuration conf, CommonTarget target)
    {
        conf.IncludePaths.Add(Path.Combine(kEASTLPath, "include"));

        conf.LibraryPaths.Add(Path.Combine(kEASTLPath, $"../Lib/SDL/{target.Optimization}"));
        if(target.Optimization == Optimization.Debug)
        {
            conf.LibraryFiles.Add("SDL2d");
        }
        else
        {
            conf.LibraryFiles.Add("SDL2");
        }


        // COPY DLL
        {
            var copyDirBuildStep = new Project.Configuration.BuildStepCopy(
                "[project.SharpmakeCsPath]/ThirdParty/Lib/SDL/[target.Optimization]",
                "[project.SharpmakeCsPath]/.build/[target.Name]");

            copyDirBuildStep.IsFileCopy = false;
            copyDirBuildStep.CopyPattern = "*.dll";
            conf.EventPostBuildExe.Add(copyDirBuildStep);
        }
    }
}