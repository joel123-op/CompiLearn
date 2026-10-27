import { useMemo, useState } from "react";
import { Braces, Check, ChevronRight, Cpu, FileCode2, GitBranch, Loader2, ScanText, Sparkles, Wand2, X } from "lucide-react";
import { CompileResult } from "../../services/api";

const stages = [
  { id: "lexer", label: "Lexer", icon: ScanText, description: "Breaks your source into meaningful tokens.", input: "Source code", output: "Token stream" },
  { id: "parser", label: "Parser", icon: Braces, description: "Checks whether the token sequence follows the language grammar.", input: "Tokens", output: "Parse tree" },
  { id: "ast", label: "AST", icon: GitBranch, description: "Builds a simpler tree that represents program meaning.", input: "Parse tree", output: "Abstract syntax tree" },
  { id: "semantic", label: "Semantic", icon: Sparkles, description: "Validates variable declarations, types, and scopes.", input: "AST", output: "Symbol table" },
  { id: "ir", label: "IR", icon: FileCode2, description: "Converts the program into compact intermediate instructions.", input: "Validated AST", output: "Three-address code" },
  { id: "optimize", label: "Optimize", icon: Wand2, description: "Simplifies the intermediate code without changing results.", input: "IR", output: "Optimized IR" },
  { id: "assembly", label: "Assembly", icon: Cpu, description: "Generates instructions for the educational virtual machine.", input: "Optimized IR", output: "Assembly" },
  { id: "vm", label: "Output", icon: Cpu, description: "Executes the instructions and captures your program output.", input: "Assembly", output: "Console output" },
];

type PipelineVisualizerProps = { activeStage: number; isCompiling: boolean; result: CompileResult | null };

export function PipelineVisualizer({ activeStage, isCompiling, result }: PipelineVisualizerProps) {
  const [selectedStage, setSelectedStage] = useState(0);
  const selected = stages[selectedStage];
  const artifact = useMemo(() => getArtifact(selected.id, result), [selected.id, result]);

  return (
    <section className="phase-explorer" aria-label="Compiler phase visualizer">
      <div className="phase-explorer-heading">
        <div><p className="eyebrow">Compiler journey</p><h2>Follow every phase</h2></div>
        <span>{result ? "Compilation complete" : isCompiling ? "Processing…" : "Select a phase"}</span>
      </div>
      <div className="pipeline-visualizer">
        {stages.map((stage, index) => {
          const status = getStageStatus(index, activeStage, isCompiling, result);
          const StageIcon = stage.icon;
          return (
            <div className="phase-step" key={stage.id}>
              <button className={`pipeline-stage ${status} ${selectedStage === index ? "selected" : ""}`} type="button" onClick={() => setSelectedStage(index)} aria-pressed={selectedStage === index}>
                <div className="stage-icon">{getStatusIcon(status, StageIcon)}</div>
                <span><b>{String(index + 1).padStart(2, "0")}</b>{stage.label}</span>
              </button>
              {index < stages.length - 1 && <ChevronRight className="phase-arrow" size={14} aria-hidden="true" />}
            </div>
          );
        })}
      </div>
      <div className="phase-detail">
        <div className="phase-detail-icon"><selected.icon size={22} /></div>
        <div className="phase-description"><p>Phase {selectedStage + 1} · {selected.label}</p><strong>{selected.description}</strong></div>
        <div className="phase-transform"><span>{selected.input}</span><ChevronRight size={15} /><span>{selected.output}</span></div>
        <div className="phase-artifact"><span>Live artifact</span><strong>{artifact}</strong></div>
      </div>
    </section>
  );
}

function getStageStatus(index: number, activeStage: number, isCompiling: boolean, result: CompileResult | null) {
  if (!result && !isCompiling) return "idle";
  if (isCompiling) return index < activeStage ? "done" : index === activeStage ? "running" : "idle";
  if (!result?.success && index === activeStage) return "failed";
  return result ? "done" : "idle";
}

function getStatusIcon(status: string, Icon: typeof ScanText) {
  if (status === "done") return <Check size={15} aria-hidden="true" />;
  if (status === "running") return <Loader2 size={15} aria-hidden="true" />;
  if (status === "failed") return <X size={15} aria-hidden="true" />;
  return <Icon size={15} aria-hidden="true" />;
}

function getArtifact(stage: string, result: CompileResult | null) {
  if (!result) return "Awaiting compilation";
  const values: Record<string, string> = {
    lexer: `${result.tokens.length} tokens identified`,
    parser: result.parser.status === "success" ? "Grammar accepted" : `${result.parser.errors.length} parser issue(s)`,
    ast: `${result.ast.children.length} top-level node(s)`,
    semantic: result.semantic.status === "success" ? `${result.symbol_table.length} symbol(s) validated` : `${result.semantic.errors.length} semantic issue(s)`,
    ir: `${result.ir.length} IR instruction(s)`,
    optimize: result.optimizations.length ? `${result.optimizations.length} optimization(s) applied` : "No changes required",
    assembly: `${result.assembly.length} machine instruction(s)`,
    vm: result.vm.output.length ? `${result.vm.output.length} line(s) printed` : "Execution finished",
  };
  return values[stage];
}
