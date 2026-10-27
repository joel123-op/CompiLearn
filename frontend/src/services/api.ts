import axios from "axios";

export type Token = {
  lexeme: string;
  type: string;
  line: number;
  column: number;
};

export type TreeNode = {
  name: string;
  children: TreeNode[];
};

export type CompileResult = {
  success: boolean;
  message: string;
  tokens: Token[];
  parser: {
    status: string;
    errors: Array<string | Record<string, unknown>>;
  };
  parse_tree: TreeNode;
  ast: TreeNode;
  semantic: {
    status: string;
    errors: Array<string | Record<string, unknown>>;
  };
  symbol_table: Array<{
    name: string;
    type: string;
    scope: string;
    address: number;
  }>;
  ir: string[];
  optimized_ir: string[];
  optimizations: string[];
  assembly: string[];
  vm: {
    status: string;
    steps: number;
    registers: Record<string, string | number | null>;
    memory: Record<string, string | number | null>;
    stack: Array<string | number | null>;
    output: string[];
    trace: Array<{
      pc: number;
      instruction: string;
      registers: Record<string, string | number | null>;
      memory: Record<string, string | number | null>;
      output: string[];
    }>;
    return_value: string | number | null;
    errors: Array<string | Record<string, unknown>>;
  };
  ai_explanation: {
    status: string;
    summary: string;
    likely_cause: string;
    suggested_fix: string;
    example: string;
    errors: Array<string | Record<string, unknown>>;
  };
  metrics: Record<string, number>;
};

const api = axios.create({
  baseURL: import.meta.env.VITE_API_URL ?? "http://localhost:5003",
});

export async function compileSource(source: string): Promise<CompileResult> {
  const response = await api.post<CompileResult>("/compile", { source });
  return response.data;
}
