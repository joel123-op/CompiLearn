const tabs = [
  { id: "tokens", label: "Tokens" },
  { id: "parser", label: "Parser" },
  { id: "parseTree", label: "Parse Tree" },
  { id: "ast", label: "AST" },
  { id: "semantic", label: "Semantic" },
  { id: "symbols", label: "Symbols" },
  { id: "ir", label: "IR" },
  { id: "optimization", label: "Optimize" },
  { id: "assembly", label: "Assembly" },
  { id: "vm", label: "VM" },
  { id: "ai", label: "AI Help" },
  { id: "report", label: "Report" },
  { id: "metrics", label: "Metrics" },
];

type PipelineTabsProps = {
  activeTab: string;
  onTabChange: (tab: string) => void;
};

export function PipelineTabs({ activeTab, onTabChange }: PipelineTabsProps) {
  return (
    <div className="tabs" id="pipeline" role="tablist" aria-label="Compiler stages">
      {tabs.map((tab) => (
        <button
          key={tab.id}
          className={activeTab === tab.id ? "tab active" : "tab"}
          type="button"
          role="tab"
          aria-selected={activeTab === tab.id}
          onClick={() => onTabChange(tab.id)}
        >
          {tab.label}
        </button>
      ))}
    </div>
  );
}
