import { useEffect, useMemo, useState } from "react";
import type { ReactNode } from "react";
import { ChevronLeft, ChevronRight, Download, RotateCcw } from "lucide-react";
import { CompileResult, TreeNode } from "../../services/api";

type OutputPanelProps = {
  activeTab: string;
  result: CompileResult | null;
  source: string;
};

export function OutputPanel({ activeTab, result, source }: OutputPanelProps) {
  if (!result) {
    return (
      <section className="panel-empty">
        <h2>No compilation yet</h2>
        <p>Press Compile to send the source code to the Flask API.</p>
      </section>
    );
  }

  if (activeTab === "tokens") {
    return (
      <section className="panel">
        <table>
          <thead>
            <tr>
              <th>Lexeme</th>
              <th>Token</th>
              <th>Line</th>
              <th>Column</th>
            </tr>
          </thead>
          <tbody>
            {result.tokens.map((token, index) => (
              <tr key={`${token.lexeme}-${index}`}>
                <td>{token.lexeme}</td>
                <td>{token.type}</td>
                <td>{token.line}</td>
                <td>{token.column}</td>
              </tr>
            ))}
          </tbody>
        </table>
      </section>
    );
  }

  if (activeTab === "parser") {
    return <JsonPanel title="Parser" data={result.parser} />;
  }

  if (activeTab === "parseTree") {
    return <TreePanel title="Parse Tree" node={result.parse_tree} />;
  }

  if (activeTab === "ast") {
    return <TreePanel title="AST" node={result.ast} />;
  }

  if (activeTab === "semantic") {
    return <JsonPanel title="Semantic Analysis" data={result.semantic} />;
  }

  if (activeTab === "symbols") {
    return <SymbolExplorer result={result} />;
  }

  if (activeTab === "ir") {
    return <LinesPanel title="Intermediate Code" lines={result.ir} />;
  }

  if (activeTab === "optimization") {
    return (
      <section className="panel">
        <h2>Optimization Comparison</h2>
        <div className="comparison-grid">
          <div>
            <h3>Before</h3>
            <pre>{result.ir.join("\n")}</pre>
          </div>
          <div>
            <h3>After</h3>
            <pre>{result.optimized_ir.join("\n")}</pre>
          </div>
        </div>
        {result.optimizations.length > 0 && (
          <div className="optimization-notes">
            <h3>Applied</h3>
            <ul>
              {result.optimizations.map((optimization, index) => (
                <li key={`${optimization}-${index}`}>{optimization}</li>
              ))}
            </ul>
          </div>
        )}
      </section>
    );
  }

  if (activeTab === "assembly") {
    return <LinesPanel title="Assembly" lines={result.assembly} />;
  }

  if (activeTab === "vm") {
    return <VmPanel result={result} />;
  }

  if (activeTab === "ai") {
    return <AiHelpPanel result={result} />;
  }

  if (activeTab === "report") {
    return <ReportPanel result={result} source={source} />;
  }

  return <JsonPanel title="Code Metrics" data={result.metrics} />;
}

function JsonPanel({ title, data }: { title: string; data: unknown }) {
  return (
    <section className="panel">
      <h2>{title}</h2>
      <pre>{JSON.stringify(data, null, 2)}</pre>
    </section>
  );
}

function LinesPanel({ title, lines }: { title: string; lines: string[] }) {
  return (
    <section className="panel">
      <h2>{title}</h2>
      <pre>{lines.join("\n")}</pre>
    </section>
  );
}

function TreePanel({ title, node }: { title: string; node: TreeNode }) {
  const [selectedNode, setSelectedNode] = useState<TreeNode>(node);

  useEffect(() => {
    setSelectedNode(node);
  }, [node]);

  return (
    <section className="panel">
      <h2>{title}</h2>
      <div className="explorer-layout">
        <div className="explorer-tree">
          <TreeNodeView node={node} selectedNode={selectedNode} onSelect={setSelectedNode} />
        </div>
        <div className="inspector-panel">
          <h3>Node Details</h3>
          <dl>
            <dt>Name</dt>
            <dd>{selectedNode.name}</dd>
            <dt>Children</dt>
            <dd>{selectedNode.children.length}</dd>
            <dt>Kind</dt>
            <dd>{describeNode(selectedNode.name)}</dd>
          </dl>
        </div>
      </div>
    </section>
  );
}

function TreeNodeView({
  node,
  selectedNode,
  onSelect,
}: {
  node: TreeNode;
  selectedNode: TreeNode;
  onSelect: (node: TreeNode) => void;
}) {
  return (
    <div className="tree-node">
      <button
        className={selectedNode === node ? "tree-node-button selected" : "tree-node-button"}
        type="button"
        onClick={() => onSelect(node)}
      >
        {node.name}
      </button>
      {node.children.length > 0 && (
        <div className="tree-children">
          {node.children.map((child, index) => (
            <TreeNodeView
              key={`${child.name}-${index}`}
              node={child}
              selectedNode={selectedNode}
              onSelect={onSelect}
            />
          ))}
        </div>
      )}
    </div>
  );
}

function describeNode(name: string) {
  if (name === "Program") return "Root of the source program";
  if (name === "block") return "Nested statement scope";
  if (name.startsWith("declare ")) return "Variable declaration";
  if (name === "=") return "Assignment expression";
  if (["+", "-", "*", "/", "%"].includes(name)) return "Arithmetic operation";
  if (["<", ">", "<=", ">=", "==", "!="].includes(name)) return "Comparison operation";
  if (["if", "ifelse", "while"].includes(name)) return "Control-flow statement";
  if (name === "print") return "Console output statement";
  if (name === "return") return "Function return statement";
  if (name.includes(":")) return "Parse grammar leaf";
  return "Expression or literal node";
}

function VmPanel({ result }: { result: CompileResult }) {
  const vm = result.vm;
  const [stepIndex, setStepIndex] = useState(0);
  const selectedStep = vm.trace[Math.min(stepIndex, Math.max(vm.trace.length - 1, 0))];
  const currentRegisters = selectedStep?.registers ?? vm.registers;
  const currentMemory = selectedStep?.memory ?? vm.memory;
  const currentOutput = selectedStep?.output ?? vm.output;
  const currentInstruction = selectedStep?.instruction ?? "-";
  const progressLabel = useMemo(() => {
    if (vm.trace.length === 0) {
      return "0 / 0";
    }

    return `${Math.min(stepIndex + 1, vm.trace.length)} / ${vm.trace.length}`;
  }, [stepIndex, vm.trace.length]);

  return (
    <section className="panel">
      <div className="vm-header">
        <div>
          <h2>Virtual Machine</h2>
          <p>Status: {vm.status} · Steps: {vm.steps}</p>
        </div>
        <div className="return-value">Return: {formatValue(vm.return_value)}</div>
      </div>

      <div className="vm-stepper">
        <button
          type="button"
          onClick={() => setStepIndex(0)}
          disabled={vm.trace.length === 0 || stepIndex === 0}
          title="Reset execution trace"
        >
          <RotateCcw size={16} aria-hidden="true" />
        </button>
        <button
          type="button"
          onClick={() => setStepIndex((index) => Math.max(index - 1, 0))}
          disabled={vm.trace.length === 0 || stepIndex === 0}
          title="Previous VM step"
        >
          <ChevronLeft size={16} aria-hidden="true" />
        </button>
        <div className="step-readout">
          <span>{progressLabel}</span>
          <code>{currentInstruction}</code>
        </div>
        <button
          type="button"
          onClick={() => setStepIndex((index) => Math.min(index + 1, vm.trace.length - 1))}
          disabled={vm.trace.length === 0 || stepIndex >= vm.trace.length - 1}
          title="Next VM step"
        >
          <ChevronRight size={16} aria-hidden="true" />
        </button>
      </div>

      <div className="vm-grid">
        <StateTable title="Registers" values={currentRegisters} />
        <StateTable title="Memory" values={currentMemory} />
        <ListPanel title="Stack" values={vm.stack.map(formatValue)} />
        <ListPanel title="Output" values={currentOutput} />
      </div>

      {vm.errors.length > 0 && (
        <div className="vm-errors">
          <h3>VM Errors</h3>
          <pre>{JSON.stringify(vm.errors, null, 2)}</pre>
        </div>
      )}

      <div className="trace-panel">
        <h3>Execution Trace</h3>
        <table>
          <thead>
            <tr>
              <th>PC</th>
              <th>Instruction</th>
              <th>Output</th>
            </tr>
          </thead>
          <tbody>
            {vm.trace.map((step, index) => (
              <tr
                className={index === stepIndex ? "selected-step" : ""}
                key={`${step.pc}-${step.instruction}-${index}`}
              >
                <td>{step.pc}</td>
                <td><code>{step.instruction}</code></td>
                <td>{step.output.join(", ") || "-"}</td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </section>
  );
}

function StateTable({ title, values }: { title: string; values: Record<string, string | number | null> }) {
  const entries = Object.entries(values);

  return (
    <div className="state-panel">
      <h3>{title}</h3>
      {entries.length === 0 ? (
        <p>-</p>
      ) : (
        <table>
          <tbody>
            {entries.map(([name, value]) => (
              <tr key={name}>
                <td><code>{name}</code></td>
                <td>{formatValue(value)}</td>
              </tr>
            ))}
          </tbody>
        </table>
      )}
    </div>
  );
}

function ListPanel({ title, values }: { title: string; values: string[] }) {
  return (
    <div className="state-panel">
      <h3>{title}</h3>
      {values.length === 0 ? <p>-</p> : <pre>{values.join("\n")}</pre>}
    </div>
  );
}

function SymbolExplorer({ result }: { result: CompileResult }) {
  const [selectedName, setSelectedName] = useState(result.symbol_table[0]?.name ?? "");
  const selectedSymbol = result.symbol_table.find((symbol) => symbol.name === selectedName) ?? result.symbol_table[0];
  const usageCount = selectedSymbol
    ? result.tokens.filter((token) => token.type === "Identifier" && token.lexeme === selectedSymbol.name).length
    : 0;
  const currentValue = selectedSymbol ? result.vm.memory[selectedSymbol.name] : null;

  return (
    <section className="panel">
      <h2>Symbol Explorer</h2>
      {result.symbol_table.length === 0 ? (
        <p className="muted">No symbols were generated.</p>
      ) : (
        <div className="explorer-layout">
          <div className="symbol-list">
            {result.symbol_table.map((symbol) => (
              <button
                className={symbol.name === selectedSymbol?.name ? "symbol-row selected" : "symbol-row"}
                key={`${symbol.scope}-${symbol.name}`}
                type="button"
                onClick={() => setSelectedName(symbol.name)}
              >
                <span>{symbol.name}</span>
                <code>{symbol.type}</code>
              </button>
            ))}
          </div>
          {selectedSymbol && (
            <div className="inspector-panel">
              <h3>{selectedSymbol.name}</h3>
              <dl>
                <dt>Type</dt>
                <dd>{selectedSymbol.type}</dd>
                <dt>Scope</dt>
                <dd>{selectedSymbol.scope}</dd>
                <dt>Address</dt>
                <dd>{selectedSymbol.address}</dd>
                <dt>Uses</dt>
                <dd>{usageCount}</dd>
                <dt>Current Value</dt>
                <dd>{formatValue(currentValue ?? null)}</dd>
              </dl>
            </div>
          )}
        </div>
      )}
    </section>
  );
}

function AiHelpPanel({ result }: { result: CompileResult }) {
  const explanation = result.ai_explanation;

  return (
    <section className="panel">
      <h2>AI Error Explanation</h2>
      <div className={`ai-panel ${explanation.status}`}>
        <h3>{explanation.summary}</h3>
        <dl>
          <dt>Status</dt>
          <dd>{explanation.status === "success" ? "No error detected" : "Needs attention"}</dd>
          <dt>Likely Cause</dt>
          <dd>{explanation.likely_cause}</dd>
          <dt>Suggested Fix</dt>
          <dd>{explanation.suggested_fix}</dd>
        </dl>
        {explanation.example && (
          <div>
            <h3>Example</h3>
            <pre>{explanation.example}</pre>
          </div>
        )}
      </div>
      {explanation.errors.length > 0 && (
        <div className="trace-panel">
          <h3>Raw Compiler Errors</h3>
          <pre>{JSON.stringify(explanation.errors, null, 2)}</pre>
        </div>
      )}
    </section>
  );
}

function ReportPanel({ result, source }: { result: CompileResult; source: string }) {
  const generatedAt = new Date().toLocaleString();

  return (
    <section className="panel report-panel">
      <div className="report-toolbar">
        <div>
          <h2>Compiler Report</h2>
          <p>Use the export button and choose Save as PDF.</p>
        </div>
        <button className="icon-text-button" type="button" onClick={() => window.print()}>
          <Download size={16} aria-hidden="true" />
          Export PDF
        </button>
      </div>

      <div className="print-report">
        <h1>CompilerViz Report</h1>
        <ReportSection title="Summary">
          <dl className="report-summary">
            <dt>Generated</dt>
            <dd>{generatedAt}</dd>
            <dt>Status</dt>
            <dd>{result.success ? "Successful compilation" : "Compilation has errors"}</dd>
            <dt>Message</dt>
            <dd>{result.message}</dd>
            <dt>Token Count</dt>
            <dd>{result.metrics.tokens ?? result.tokens.length}</dd>
          </dl>
        </ReportSection>
        <ReportSection title="Source Code">
          <pre>{source}</pre>
        </ReportSection>
        <ReportSection title="AI Explanation">
          <p>{result.ai_explanation.summary}</p>
          <p>{result.ai_explanation.suggested_fix}</p>
        </ReportSection>
        <ReportSection title="Tokens">
          <pre>{JSON.stringify(result.tokens, null, 2)}</pre>
        </ReportSection>
        <ReportSection title="Parse Tree">
          <pre>{JSON.stringify(result.parse_tree, null, 2)}</pre>
        </ReportSection>
        <ReportSection title="AST">
          <pre>{JSON.stringify(result.ast, null, 2)}</pre>
        </ReportSection>
        <ReportSection title="Semantic And Symbols">
          <pre>{JSON.stringify({ semantic: result.semantic, symbols: result.symbol_table }, null, 2)}</pre>
        </ReportSection>
        <ReportSection title="Intermediate Code">
          <pre>{result.ir.join("\n")}</pre>
        </ReportSection>
        <ReportSection title="Optimized TAC">
          <pre>{result.optimized_ir.join("\n")}</pre>
        </ReportSection>
        <ReportSection title="Assembly">
          <pre>{result.assembly.join("\n")}</pre>
        </ReportSection>
        <ReportSection title="VM Output And Metrics">
          <pre>{JSON.stringify({ output: result.vm.output, return_value: result.vm.return_value, metrics: result.metrics }, null, 2)}</pre>
        </ReportSection>
      </div>
    </section>
  );
}

function ReportSection({ title, children }: { title: string; children: ReactNode }) {
  return (
    <section className="report-section">
      <h3>{title}</h3>
      {children}
    </section>
  );
}

function formatValue(value: string | number | null) {
  if (value === null || value === undefined) {
    return "-";
  }

  return String(value);
}
