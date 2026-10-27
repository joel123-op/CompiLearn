import { Activity, Braces, CheckCircle2, CircleAlert, FileCode2 } from "lucide-react";
import { CompileResult } from "../../services/api";

type CompileSnapshotProps = {
  result: CompileResult | null;
  isCompiling: boolean;
};

export function CompileSnapshot({ result, isCompiling }: CompileSnapshotProps) {
  const errors = result
    ? result.parser.errors.length + result.semantic.errors.length + result.vm.errors.length
    : 0;
  const values = result
    ? [
        { label: "Tokens", value: result.metrics.tokens, icon: <FileCode2 size={17} />, tone: "blue" },
        { label: "Symbols", value: result.symbol_table.length, icon: <Braces size={17} />, tone: "purple" },
        { label: "Stages", value: "8/8", icon: <Activity size={17} />, tone: "teal" },
        { label: errors ? "Issues" : "Status", value: errors || "Clear", icon: errors ? <CircleAlert size={17} /> : <CheckCircle2 size={17} />, tone: errors ? "rose" : "green" },
      ]
    : [
        { label: "Tokens", value: "—", icon: <FileCode2 size={17} />, tone: "blue" },
        { label: "Symbols", value: "—", icon: <Braces size={17} />, tone: "purple" },
        { label: "Stages", value: isCompiling ? "…" : "0/8", icon: <Activity size={17} />, tone: "teal" },
        { label: "Status", value: isCompiling ? "Running" : "Ready", icon: <CheckCircle2 size={17} />, tone: "green" },
      ];

  return (
    <section className="compile-snapshot" aria-label="Compilation summary">
      <div className="snapshot-label"><span className={isCompiling ? "live-dot active" : "live-dot"} /> Compilation snapshot</div>
      <div className="snapshot-grid">
        {values.map((item) => (
          <div className={`snapshot-stat ${item.tone}`} key={item.label}>
            <span className="snapshot-icon">{item.icon}</span>
            <div><strong>{item.value}</strong><span>{item.label}</span></div>
          </div>
        ))}
      </div>
    </section>
  );
}
