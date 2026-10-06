export declare class DatabaseService {
    execute(query: string): Promise<{
        ok: boolean;
        output: string;
    }>;
    tables(): {
        tables: string[];
    };
}
