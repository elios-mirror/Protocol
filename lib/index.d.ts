export class EliosProtocol {
  is_sdk: string;
  socket_path: string;
  receive(callback: (message: string, sender_id: string, command_type: number) => {}): void;
  send(message: string, command_type?: number): void;
  close(): void;
}

export function createConnection(socket_path: string, sender_id: string, sdk: boolean): EliosProtocol;