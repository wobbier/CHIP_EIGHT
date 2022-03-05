using Sharpmake;

[module: Sharpmake.Include("Tools/CommonTarget.sharpmake.cs")]
[module: Sharpmake.Include("Tools/BaseProject.sharpmake.cs")]
[module: Sharpmake.Include("ThirdParty/SDL.sharpmake.cs")]

[Generate]
public class BaseGameProject : BaseProject
{
    public BaseGameProject()
        : base()
    {
        Name = "ChipEight";
        SourceRootPath = @"Source";
    }

    public override void ConfigureAll(Project.Configuration conf, CommonTarget target)
    {
        base.ConfigureAll(conf, target);
        conf.Output = Configuration.OutputType.Exe;
        conf.SolutionFolder = "App";
        conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = "$(OutDir)";

        conf.IncludePaths.Add("[project.SourceRootPath]");
        SDL.ConfigureAll(conf, target);
    }
}

[Generate]
public class BaseGameSolution : Solution
{
    public BaseGameSolution()
        : base(typeof(CommonTarget))
    {
        Name = "BaseGameSolution";

        AddTargets(CommonTarget.GetDefaultTargets());

        IsFileNameToLower = false;
    }

    [Configure]
    public virtual void ConfigureAll(Solution.Configuration conf, CommonTarget target)
    {
        conf.SolutionPath = @"[solution.SharpmakeCsPath]";
        conf.SolutionFileName = "[solution.Name]";

        conf.AddProject<BaseGameProject>(target);

        if(target.Platform == Platform.win64)
        {
            conf.AddProject<SharpmakeProjectBase>(target);
        }
    }
}

[Generate]
public class SharpmakeProjectBase : CSharpProject
{
    public SharpmakeProjectBase()
        : base(typeof(CommonTarget))
    {
        Name = "SharpmakeProject";
        SourceRootPath = @"./";

        ProjectSchema = CSharpProjectSchema.NetFramework;
        string[] things = { ".xml", ".map", ".config", ".bat", ".txt", ".xsd", ".h.template", ".resx", ".cur" };
        NoneExtensions.Remove(things);
        SourceFilesExtensions = new Strings("Engine.sharpmake.cs");
        
        ContentExtension.Add("GenerateSolution.bat", "macOS.yml", "Windows.yml");

        SourceFiles.Add(@"[project.SharpmakeCsPath]/ChipEight.sharpmake.cs");
        SourceFiles.Add(@"[project.SharpmakeCsPath]/Tools/CommonTarget.sharpmake.cs");
        SourceFiles.Add(@"[project.SharpmakeCsPath]/Tools/BaseProject.sharpmake.cs");
        SourceFiles.Add(@"[project.SharpmakeCsPath]/ThirdParty/SDL.sharpmake.cs");

        DependenciesCopyLocal = DependenciesCopyLocalTypes.None;
        AddTargets(CommonTarget.GetDefaultTargets());
    }

    [Configure]
    public virtual void ConfigureAll(Configuration conf, CommonTarget target)
    {
        conf.Output = Configuration.OutputType.DotNetClassLibrary;
        conf.ProjectFileName = @"[project.Name]_[target.Platform]";
        conf.SolutionFolder = "Tools";

        conf.TargetPath = "$(SolutionDir).build/Sharpmake/[target.Optimization]/";
        conf.ProjectPath = @"[project.SharpmakeCsPath]/.tmp/project/[target.Framework]";
        CSharpProjectExtensions.AddAspNetReferences(conf);
        conf.ReferencesByPath.Add(@"[project.SharpmakeCsPath]/Tools/Sharpmake/Sharpmake.dll");
        conf.ReferencesByPath.Add(@"[project.SharpmakeCsPath]/Tools/Sharpmake/Sharpmake.Generators.dll");
    }
}

public static class Main
{
    [Sharpmake.Main]
    public static void SharpmakeMain(Sharpmake.Arguments arguments)
    {
        KitsRootPaths.SetUseKitsRootForDevEnv(DevEnv.vs2019, KitsRootEnum.KitsRoot10, Options.Vc.General.WindowsTargetPlatformVersion.v10_0_19041_0);
        arguments.Generate<BaseGameSolution>();
    }
}