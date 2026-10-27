import Editor from "@monaco-editor/react";

type CodeEditorProps = {
  value: string;
  onChange: (value: string) => void;
};

export function CodeEditor({ value, onChange }: CodeEditorProps) {
  return (
    <div className="editor-frame" id="editor">
      <Editor
        height="100%"
        defaultLanguage="c"
        theme="vs-dark"
        value={value}
        onChange={(nextValue) => onChange(nextValue ?? "")}
        options={{
          minimap: { enabled: false },
          fontSize: 15,
          lineNumbers: "on",
          scrollBeyondLastLine: false,
          tabSize: 2,
          automaticLayout: true,
          padding: { top: 16, bottom: 16 },
        }}
      />
    </div>
  );
}
