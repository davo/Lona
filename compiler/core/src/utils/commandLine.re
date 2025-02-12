open Operators;

type inputReference =
  | File(string)
  | Stdin;

module Command = {
  type t =
    | Version
    | Config(string)
    | Colors(Types.compilerTarget, inputReference)
    | TextStyles(Types.compilerTarget, inputReference)
    | Shadows(Types.compilerTarget, inputReference)
    | Types(Types.compilerTarget, string)
    | Logic(Types.compilerTarget, inputReference, string)
    | Component(Types.compilerTarget, string)
    | Workspace(Types.compilerTarget, string, string);

  exception Unknown(string);
};

module Parse = {
  let target: string => option(Types.compilerTarget) =
    name =>
      switch (name) {
      | "js" => Some(Types.JavaScript)
      | "swift" => Some(Types.Swift)
      | "xml" => Some(Types.Xml)
      | "reason" => Some(Types.Reason)
      | _ => None
      };
};

module Arguments = {
  let setRef = (container: ref('a), f: 'b => 'a, arg: 'b) =>
    container := f(arg);

  let setOptionalString = (container: ref(option(string))) =>
    setRef(container, string => Some(string));

  module Spec = {
    type t = (Arg.key, Arg.spec, Arg.doc);

    /* Universal options */

    let target = container => (
      "--target",
      Arg.Symbol(
        ["js", "swift", "xml", "reason"],
        Parse.target |> setRef(container),
      ),
      " Output language",
    );

    let workspace = (container: ref(option(string))) => (
      "--workspace",
      Arg.String(setOptionalString(container)),
      "[path] Workspace directory path",
    );

    let output = (container: ref(option(string))) => (
      "--output",
      Arg.String(setOptionalString(container)),
      "[path] Output directory path",
    );

    let input = (container: ref(option(string))) => (
      "--input",
      Arg.String(setOptionalString(container)),
      "[path] Input file path (for commands that take an input file)",
    );

    let filterComponents = (container: ref(option(string))) => (
      "--filterComponents",
      Arg.String(setOptionalString(container)),
      "[pattern] Only generate components matching this string (converted to a regular expression)",
    );

    /* Target options */

    let framework = (container: ref(option(string))) => (
      "--framework",
      Arg.Symbol(
        ["uikit", "appkit", "reactdom", "reactnative", "reactsketchapp"],
        setOptionalString(container),
      ),
      " UI framework",
    );

    /* Swift options */

    let swiftVersion = (container: ref(option(string))) => (
      "--swiftVersion",
      Arg.Symbol(["4", "5"], setOptionalString(container)),
      " Swift version",
    );

    let debugConstraints = (container: ref(bool)) => (
      "--debugConstraints",
      Arg.Set(container),
      " Generate constraint IDs for easier debugging",
    );

    let generateCollectionView = (container: ref(bool)) => (
      "--generateCollectionView",
      Arg.Set(container),
      " Generate experimental UICollectionView subclass that renders LonaModels as cells",
    );

    let typePrefix = (container: ref(option(string))) => (
      "--typePrefix",
      Arg.String(setOptionalString(container)),
      "[prefix] Generated type prefix",
    );

    /* JS options */

    let styleFramework = (container: ref(option(string))) => (
      "--styleFramework",
      Arg.Symbol(
        ["none", "styledcomponents"],
        setOptionalString(container),
      ),
      " Style framework",
    );

    let styledComponentsVersion = (container: ref(option(string))) => (
      "--styledComponentsVersion",
      Arg.Symbol(["3", "latest"], setOptionalString(container)),
      " styled-components library version",
    );

    let minimumIeSupport = (container: ref(option(string))) => (
      "--minimumIeSupport",
      Arg.Symbol(["none", "11"], setOptionalString(container)),
      " Minimum version of IE supported",
    );
  };

  let usageMessage = {|
The Lona compiler generates code from a Lona workspace (a directory with a lona.json manifest file).

The following commands are available:

  workspace
  component
  colors
  textStyles
  shadows
  logic
  types
  config
  version

Type `lonac [command]` to see which options are required for that command. The following options are available:
|};

  type scanResult = {
    command: Command.t,
    options: Options.options,
  };

  let scan = (arguments: list(string)): scanResult => {
    /* Remove the program path */
    let arguments = arguments |> List.tl;

    /* Accept "=" in options for backwards compatibility */
    let arguments =
      arguments
      |> List.map(arg =>
           switch (arg |> Js.String.match([%re "/(--\\w+)=(.+)$/"])) {
           | Some(matches) => [matches[1], matches[2]]
           | None => [arg]
           }
         )
      |> List.concat;

    let positionalArgs: ref(list(string)) = ref([]);

    /* Main options */
    let targetRef: ref(option(Types.compilerTarget)) = ref(None);
    let workspaceRef: ref(option(string)) = ref(None);
    let outputRef: ref(option(string)) = ref(None);
    let inputRef: ref(option(string)) = ref(None);
    let filterComponentsRef: ref(option(string)) = ref(None);

    /* Target options */
    let frameworkRef: ref(option(string)) = ref(None);

    /* Swift options */
    let swiftVersionRef: ref(option(string)) = ref(None);
    let debugContraintsRef: ref(bool) = ref(false);
    let generateCollectionViewRef: ref(bool) = ref(false);
    let typePrefixRef: ref(option(string)) = ref(None);

    /* JS options */
    let styleFrameworkRef: ref(option(string)) = ref(None);
    let styledComponentsVersionRef: ref(option(string)) = ref(None);
    let minimumIeSupportRef: ref(option(string)) = ref(None);

    let spec =
      Arg.align([
        Spec.target(targetRef),
        Spec.workspace(workspaceRef),
        Spec.output(outputRef),
        Spec.input(inputRef),
        Spec.filterComponents(filterComponentsRef),
        /* Target options */
        Spec.framework(frameworkRef),
        /* Swift options */
        Spec.swiftVersion(swiftVersionRef),
        Spec.debugConstraints(debugContraintsRef),
        Spec.generateCollectionView(generateCollectionViewRef),
        Spec.typePrefix(typePrefixRef),
        /* JS options */
        Spec.styleFramework(styleFrameworkRef),
        Spec.styledComponentsVersion(styledComponentsVersionRef),
        Spec.minimumIeSupport(minimumIeSupportRef),
      ]);

    switch (
      Arg.parse_argv(
        arguments |> Array.of_list,
        spec,
        arg => positionalArgs := positionalArgs^ @ [arg],
        usageMessage,
      )
    ) {
    | _ => ()
    | exception (Arg.Bad(message)) => raise(Command.Unknown(message))
    | exception (Arg.Help(message)) => raise(Command.Unknown(message))
    };

    /* Convert positional args to options, for backwards compatibility */
    switch (positionalArgs^) {
    | ["config", workspace] => workspaceRef := Some(workspace)

    | ["colors", target]
    | ["textStyles", target]
    | ["shadows", target] => targetRef := Parse.target(target)

    | ["colors", target, input]
    | ["textStyles", target, input]
    | ["shadows", target, input]
    | ["types", target, input]
    | ["component", target, input] =>
      targetRef := Parse.target(target);
      inputRef := Some(input);

    | ["workspace", target, workspace, output] =>
      targetRef := Parse.target(target);
      workspaceRef := Some(workspace);
      outputRef := Some(output);
    | _ => ()
    };

    if (List.length(positionalArgs^) == 0) {
      raise(
        Command.Unknown(
          "No command given.\n" ++ Arg.usage_string(spec, usageMessage),
        ),
      );
    };

    let command =
      switch (List.hd(positionalArgs^)) {
      | "version" => Command.Version
      | "config" =>
        switch (workspaceRef^) {
        | Some(workspace) => Command.Config(workspace)
        | None =>
          raise(
            Command.Unknown("Missing workspace directory path (--workspace)"),
          )
        }
      | "colors" =>
        switch (targetRef^, inputRef^) {
        | (None, _) =>
          raise(Command.Unknown("Missing output target (--target)"))
        | (Some(target), None) => Command.Colors(target, Stdin)
        | (Some(target), Some(path)) => Command.Colors(target, File(path))
        }
      | "textStyles" =>
        switch (targetRef^, inputRef^) {
        | (None, _) =>
          raise(Command.Unknown("Missing output target (--target)"))
        | (Some(target), None) => Command.TextStyles(target, Stdin)
        | (Some(target), Some(path)) =>
          Command.TextStyles(target, File(path))
        }
      | "shadows" =>
        switch (targetRef^, inputRef^) {
        | (None, _) =>
          raise(Command.Unknown("Missing output target (--target)"))
        | (Some(target), None) => Command.Shadows(target, Stdin)
        | (Some(target), Some(path)) =>
          Command.Shadows(target, File(path))
        }
      | "types" =>
        switch (targetRef^, inputRef^) {
        | (None, _) =>
          raise(Command.Unknown("Missing output target (--target)"))
        | (_, None) =>
          raise(Command.Unknown("Missing input path (--input)"))
        | (Some(target), Some(path)) => Command.Types(target, path)
        }
      | "logic" =>
        switch (targetRef^, inputRef^) {
        | (None, _) =>
          raise(Command.Unknown("Missing output target (--target)"))
        | (Some(target), None) =>
          Command.Logic(target, Stdin, workspaceRef^ %? Node.Process.cwd())
        | (Some(target), Some(path)) =>
          Command.Logic(target, File(path), workspaceRef^ %? path)
        }
      | "component" =>
        switch (targetRef^, inputRef^) {
        | (None, _) =>
          raise(Command.Unknown("Missing output target (--target)"))
        | (_, None) =>
          raise(Command.Unknown("Missing input path (--input)"))
        | (Some(target), Some(path)) => Command.Component(target, path)
        }
      | "workspace" =>
        switch (targetRef^, workspaceRef^, outputRef^) {
        | (None, _, _) =>
          raise(Command.Unknown("Missing output target (--target)"))
        | (_, None, _) =>
          raise(
            Command.Unknown("Missing workspace directory path (--workspace)"),
          )
        | (_, _, None) =>
          raise(Command.Unknown("Missing output directory path (--output)"))
        | (Some(target), Some(workspacePath), Some(outputPath)) =>
          Command.Workspace(target, workspacePath, outputPath)
        }
      | name =>
        raise(
          Command.Unknown(
            "Unrecognized command: "
            ++ name
            ++ "\n"
            ++ Arg.usage_string(spec, usageMessage),
          ),
        )
      };

    {
      command,
      options: {
        preset: Options.Standard,
        filterComponents: filterComponentsRef^,
        swift: {
          framework:
            switch (frameworkRef^) {
            | Some("appkit") => Swift.Options.AppKit
            | _ => Swift.Options.UIKit
            },
          debugConstraints: debugContraintsRef^,
          typePrefix:
            switch (typePrefixRef^) {
            | Some(value) => value
            | _ => ""
            },
          generateCollectionView: generateCollectionViewRef^,
          swiftVersion:
            switch (swiftVersionRef^) {
            | Some("4") => V4
            | Some("5") => V5
            | _ => V5
            },
        },
        javaScript: {
          framework:
            switch (frameworkRef^) {
            | Some("reactsketchapp") => JavaScriptOptions.ReactSketchapp
            | Some("reactdom") => JavaScriptOptions.ReactDOM
            | _ => JavaScriptOptions.ReactNative
            },
          styleFramework:
            switch (styleFrameworkRef^) {
            | Some("styledcomponents") => JavaScriptOptions.StyledComponents
            | _ => JavaScriptOptions.None
            },
          styledComponentsVersion:
            switch (styledComponentsVersionRef^) {
            | Some(value) when Js.String.startsWith("3", value) => V3
            | _ => Latest
            },
          minimumIeSupport:
            switch (minimumIeSupportRef^) {
            | Some("11") => IE11
            | _ => None
            },
        },
      },
    };
  };
};