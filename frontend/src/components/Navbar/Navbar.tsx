import { Blocks } from "lucide-react";

export function Navbar() {
  return (
    <header className="navbar">
      <div className="brand">
        <Blocks size={24} aria-hidden="true" />
        <span>CompilerViz</span>
      </div>
      <nav aria-label="Main navigation">
        <a href="#editor">Editor</a>
        <a href="#pipeline">Pipeline</a>
        <a href="../docs/phase-plan.md">Docs</a>
      </nav>
    </header>
  );
}
