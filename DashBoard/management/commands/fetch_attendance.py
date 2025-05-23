# DashBoard/management/commands/fetch_attendance.py

from django.core.management.base import BaseCommand
from DashBoard.models import MemberInfo
import pandas as pd
import re
import os

class Command(BaseCommand):
    help = '엑셀 파일에서 출석일수/회의일수를 읽어와 MemberInfo.attendance_rate 에 업데이트합니다.'

    def add_arguments(self, parser):
        parser.add_argument(
    '--path',
    type=str,
    default=r'C:\Users\lenovo\Desktop\attendance.xlsx',
    help='출석 파일(.xlsx) 경로를 지정합니다.'
)
    def handle(self, *args, **options):
        excel_path = options['path']
        if not os.path.isfile(excel_path):
            self.stderr.write(self.style.ERROR(f"❌ 파일을 찾을 수 없습니다: {excel_path}"))
            return

        # 1) 엑셀 읽기 (header=2 → 0,1행 스킵, 2행을 컬럼명으로)
        try:
            df = pd.read_excel(excel_path, sheet_name='22대', header=2)
        except Exception as e:
            self.stderr.write(self.style.ERROR(f"❌ 엑셀 읽기 실패: {e}"))
            return

        # 2) 불필요한 Unnamed 컬럼 제거
        df = df.loc[:, ~df.columns.str.contains('^Unnamed')]

        # 3) 컬럼명 확인 및 타겟 컬럼 추출
        #    기본 세션 요약(회의일수, 출석) → .1 이 붙지 않은 첫 번째 그룹
        #    22대 총계 요약(회의일수.1, 출석.1) → 두 번째 그룹
        cols = df.columns.tolist()
        total_cols  = [c for c in cols if c.startswith('회의일수')]
        attend_cols = [c for c in cols if c.startswith('출석')]

        if len(total_cols) < 2 or len(attend_cols) < 2:
            self.stderr.write(self.style.ERROR(
                f"❌ 예상한 요약 컬럼을 찾을 수 없습니다. "
                f"회의일수 컬럼: {total_cols}, 출석 컬럼: {attend_cols}"
            ))
            return

        total_col  = total_cols[1]   # e.g. '회의일수.1'
        attend_col = attend_cols[1]  # e.g. '출석.1'

        updated = 0
        for _, row in df.iterrows():
            raw_name = str(row.get('의원명', '')).strip()
            # 괄호 안 의원보좌관 등 제거
            name = re.sub(r'\s*\(.*?\)', '', raw_name)

            # 숫자형으로 변환
            try:
                total_days = int(row[total_col])
                attended   = int(row[attend_col])
            except Exception:
                continue

            if total_days <= 0:
                continue

            rate = round(attended / total_days * 100, 2)

            # DB 업데이트
            try:
                member = MemberInfo.objects.get(name=name)
            except MemberInfo.DoesNotExist:
                self.stderr.write(self.style.WARNING(f"⚠ '{name}' 의원이 DB에 없습니다."))
                continue

            member.attendance_rate = rate
            member.save(update_fields=['attendance_rate'])
            updated += 1
            self.stdout.write(f"✅ {name}: {attended}/{total_days} → {rate}%")

        self.stdout.write(self.style.SUCCESS(f"출석률 업데이트 완료: 총 {updated}명 처리"))