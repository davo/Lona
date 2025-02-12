//
//  LogicViewController.swift
//  LonaStudio
//
//  Created by Devin Abbott on 6/5/19.
//  Copyright © 2019 Devin Abbott. All rights reserved.
//

import AppKit
import Logic

// MARK: - LogicViewController

class LogicViewController: NSViewController {

    // MARK: Lifecycle

    override init(nibName nibNameOrNil: NSNib.Name?, bundle nibBundleOrNil: Bundle?) {
        super.init(nibName: nil, bundle: nil)

        setUpViews()
        setUpConstraints()

        update()
    }

    required init?(coder: NSCoder) {
        super.init(coder: coder)

        setUpViews()
        setUpConstraints()

        update()
    }

    // MARK: Public

    public var rootNode: LGCSyntaxNode = .topLevelDeclarations(
        .init(
            id: UUID(),
            declarations: .init([.makePlaceholder()])
        )
    ) { didSet { update() } }

    public var onChangeRootNode: ((LGCSyntaxNode) -> Void)?

    // MARK: Private

    private let logicEditor = LogicEditor()
    private let infoBar = InfoBar()
    private let divider = Divider()
    private let containerView = NSBox()

    private var colorValues: [UUID: String] = [:]

    private let editorDisplayStyles: [LogicFormattingOptions.Style] = [.visual, .natural]

    private func setUpViews() {
        containerView.boxType = .custom
        containerView.borderType = .noBorder
        containerView.contentViewMargins = .zero

        containerView.addSubview(logicEditor)
        containerView.addSubview(infoBar)
        containerView.addSubview(divider)

        infoBar.fillColor = Colors.contentBackground

        divider.fillColor = NSSplitView.defaultDividerColor

        logicEditor.fillColor = Colors.contentBackground
        logicEditor.canvasStyle.textMargin = .init(width: 10, height: 6)

        logicEditor.formattingOptions = LogicFormattingOptions(
            style: LogicViewController.formattingStyle,
            getColor: { [unowned self] id in
                guard let colorString = self.colorValues[id],
                    let color = NSColor.parse(css: colorString) else { return nil }
                return (colorString, color)
            }
        )

        infoBar.dropdownIndex = editorDisplayStyles.firstIndex(of: LogicViewController.formattingStyle) ?? 0
        infoBar.dropdownValues = editorDisplayStyles.map { $0.displayName }
        infoBar.onChangeDropdownIndex = { [unowned self] index in
            LogicViewController.formattingStyle = self.editorDisplayStyles[index]
            let newFormattingOptions = self.logicEditor.formattingOptions
            newFormattingOptions.style = self.editorDisplayStyles[index]
            self.logicEditor.formattingOptions = newFormattingOptions
            self.infoBar.dropdownIndex = index
        }

        self.view = containerView
    }

    private func setUpConstraints() {
        containerView.translatesAutoresizingMaskIntoConstraints = false
        logicEditor.translatesAutoresizingMaskIntoConstraints = false
        infoBar.translatesAutoresizingMaskIntoConstraints = false
        divider.translatesAutoresizingMaskIntoConstraints = false

        logicEditor.topAnchor.constraint(equalTo: containerView.topAnchor).isActive = true
        logicEditor.leadingAnchor.constraint(equalTo: containerView.leadingAnchor).isActive = true
        logicEditor.trailingAnchor.constraint(equalTo: containerView.trailingAnchor).isActive = true

        logicEditor.bottomAnchor.constraint(equalTo: divider.topAnchor).isActive = true

        divider.heightAnchor.constraint(equalToConstant: 1).isActive = true
        divider.leadingAnchor.constraint(equalTo: containerView.leadingAnchor).isActive = true
        divider.trailingAnchor.constraint(equalTo: containerView.trailingAnchor).isActive = true

        divider.bottomAnchor.constraint(equalTo: infoBar.topAnchor).isActive = true

        infoBar.bottomAnchor.constraint(equalTo: containerView.bottomAnchor).isActive = true
        infoBar.leadingAnchor.constraint(equalTo: containerView.leadingAnchor).isActive = true
        infoBar.trailingAnchor.constraint(equalTo: containerView.trailingAnchor).isActive = true
    }

    private static func makePreludeProgram() -> LGCProgram {
        return .init(
            id: UUID(),
            block: .init(
                [
                    .declaration(
                        id: UUID(),
                        content: .importDeclaration(id: UUID(), name: .init(id: UUID(), name: "Prelude"))
                    )
                ]
            )
        )
    }

    private func evaluate() {
        let (scopeContext, unificationContext, substitutionResult) = StandardConfiguration.compile(rootNode)

        guard let substitution = try? substitutionResult.get() else { return }

        guard let evaluationContext = try? Compiler.evaluate(
            rootNode,
            rootNode: rootNode,
            scopeContext: scopeContext,
            unificationContext: unificationContext,
            substitution: substitution,
            context: .init()
            ).get() else { return }

        evaluationContext.values.forEach { id, value in
            if let colorString = value.colorString {
                colorValues[id] = colorString
            }
        }
    }

    private func update() {
        logicEditor.rootNode = rootNode

        evaluate()

        logicEditor.onChangeRootNode = { [unowned self] newRootNode in
            self.onChangeRootNode?(newRootNode)
            return true
        }

        logicEditor.suggestionsForNode = { rootNode, node, query in
            guard let root = LGCProgram.make(from: rootNode) else { return [] }

            let program: LGCSyntaxNode = .program(
                LGCProgram.join(programs: [LogicViewController.makePreludeProgram(), root])
                    .expandImports(importLoader: Library.load)
            )

            return StandardConfiguration.suggestions(rootNode: program, node: node, query: query)
                ?? LogicEditor.defaultSuggestionsForNode(rootNode, node, query)
        }
    }

    private static var formattingStyleKey = "Logic editor style"

    static var formattingStyle: LogicFormattingOptions.Style {
        get {
            guard let rawValue = UserDefaults.standard.string(forKey: formattingStyleKey),
                let value = LogicFormattingOptions.Style(rawValue: rawValue) else {
                return LogicFormattingOptions.Style.visual
            }
            return value
        }
        set {
            UserDefaults.standard.set(newValue.rawValue, forKey: formattingStyleKey)
        }
    }
}
