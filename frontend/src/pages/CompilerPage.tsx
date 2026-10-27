import { ChangeEvent, useEffect, useMemo, useRef, useState } from "react";
import { FileUp, LoaderCircle, Play, Sparkles, Terminal } from "lucide-react";
import { getDocument, GlobalWorkerOptions } from "pdfjs-dist";
import pdfWorker from "pdfjs-dist/build/pdf.worker.min.mjs?url";
import { CodeEditor } from "../components/Editor/CodeEditor";
import { CompileSnapshot } from "../components/CompileSnapshot/CompileSnapshot";
import { Navbar } from "../components/Navbar/Navbar";
import { OutputPanel } from "../components/OutputPanel/OutputPanel";
import { PipelineTabs } from "../components/PipelineTabs/PipelineTabs";
import { PipelineVisualizer } from "../components/PipelineVisualizer/PipelineVisualizer";
import { CompileResult, compileSource } from "../services/api";

const starterCode = `int a = 5;
int b = 10;
int c = a + b;
print(c);`;

GlobalWorkerOptions.workerSrc = pdfWorker;

export function CompilerPage() {
  const [source, setSource] = useState(starterCode);
  const [result, setResult] = useState<CompileResult | null>(null);
  const [activeTab, setActiveTab] = useState("tokens");
  const [isCompiling, setIsCompiling] = useState(false);
  const [activeStage, setActiveStage] = useState(0);
  const [error, setError] = useState<string | null>(null);
  const [isReadingPdf, setIsReadingPdf] = useState(false);
  const [pdfName, setPdfName] = useState<string | null>(null);
  const fileInputRef = useRef<HTMLInputElement>(null);

  const statusText = useMemo(() => {
    if (error) return error;
    if (result) return result.message;
    return "Ready to compile.";
  }, [error, result]);

  useEffect(() => {
    if (!isCompiling) {
      return undefined;
    }

    const timer = window.setInterval(() => {
      setActiveStage((stage) => (stage >= 7 ? 7 : stage + 1));
    }, 180);

    return () => window.clearInterval(timer);
  }, [isCompiling]);

  async function handleCompile() {
    setIsCompiling(true);
    setError(null);
    setResult(null);
    setActiveStage(0);

    try {
      const compileResult = await compileSource(source);
      setResult(compileResult);
      setActiveStage(compileResult.success ? 7 : 0);
    } catch {
      setError("Could not reach the compiler API. Start the Flask backend on port 5003.");
      setActiveStage(0);
    } finally {
      setIsCompiling(false);
    }
  }

  async function handlePdfUpload(event: ChangeEvent<HTMLInputElement>) {
    const file = event.target.files?.[0];
    event.target.value = "";

    if (!file) return;
    if (file.type !== "application/pdf" && !file.name.toLowerCase().endsWith(".pdf")) {
      setError("Please choose a PDF file.");
      return;
    }
    if (file.size > 10 * 1024 * 1024) {
      setError("Please choose a PDF smaller than 10 MB.");
      return;
    }

    setIsReadingPdf(true);
    setError(null);
    try {
      const document = await getDocument({ data: await file.arrayBuffer() }).promise;
      const pages = await Promise.all(
        Array.from({ length: document.numPages }, async (_, index) => {
          const page = await document.getPage(index + 1);
          const content = await page.getTextContent();
          return content.items
            .map((item) => ("str" in item ? item.str : ""))
            .join(" ")
            .replace(/\s*;\s*/g, ";\n")
            .replace(/\s*\{\s*/g, " {\n")
            .replace(/\s*\}\s*/g, "\n}\n")
            .trim();
        }),
      );
      const extractedCode = pages.filter(Boolean).join("\n\n");
      if (!extractedCode) throw new Error("No selectable text");
      setSource(extractedCode);
      setPdfName(file.name);
      setResult(null);
      setActiveStage(0);
    } catch {
      setError("We could not read code from this PDF. Use a text-based PDF (not a scanned image).");
    } finally {
      setIsReadingPdf(false);
    }
  }

  return (
    <main className="app-shell">
      <Navbar />

      <section className="hero-copy">
        <div>
          <p className="eyebrow"><Sparkles size={14} aria-hidden="true" /> Interactive compiler lab</p>
          <h1>See your code come to life.</h1>
          <p>Upload code from a PDF or write it below, then follow every stage from tokens to program output.</p>
        </div>
      </section>

      <section className="workspace">
        <div className="editor-pane">
          <div className="pane-toolbar">
            <div>
              <p className="eyebrow">Source workspace</p>
              <h2>Tiny C code</h2>
            </div>
            <div className="toolbar-actions">
              <input ref={fileInputRef} className="visually-hidden" type="file" accept="application/pdf,.pdf" onChange={handlePdfUpload} />
              <button className="upload-button" type="button" onClick={() => fileInputRef.current?.click()} disabled={isReadingPdf}>
                {isReadingPdf ? <LoaderCircle className="spin-icon" size={17} /> : <FileUp size={17} />}
                {isReadingPdf ? "Reading PDF…" : "Upload PDF"}
              </button>
              <button className="compile-button" onClick={handleCompile} disabled={isCompiling || isReadingPdf}>
                <Play size={18} aria-hidden="true" />
                {isCompiling ? "Compiling…" : "Run code"}
              </button>
            </div>
          </div>

          <div className="source-note">{pdfName ? <><FileUp size={14} /> Loaded from {pdfName}</> : "Paste C code or upload a text-based PDF. Supports a beginner C subset, including main, if/else, and printf."}</div>

          <CodeEditor value={source} onChange={setSource} />
        </div>

        <div className="output-pane">
          <div className="status-bar">{statusText}</div>
          <section className="program-output" aria-live="polite">
            <div className="output-heading"><Terminal size={18} /><div><p>Program output</p><span>Virtual machine console</span></div></div>
            <pre>{result ? (result.vm.output.length ? result.vm.output.join("\n") : "Program completed with no printed output.") : "Run your code to see its output here."}</pre>
          </section>
          <CompileSnapshot result={result} isCompiling={isCompiling} />
          <PipelineVisualizer activeStage={activeStage} isCompiling={isCompiling} result={result} />
          <PipelineTabs activeTab={activeTab} onTabChange={setActiveTab} />
          <OutputPanel activeTab={activeTab} result={result} source={source} />
        </div>
      </section>
    </main>
  );
}
