def self.make_modrm(r32, rm32)
  # regビット
  tmp = r32.bit * 8

  sib = ""
  disp = ""

  # rm32の型により分岐
  case rm32
  # reg to reg
  when Register32
    tmp += 0xc0 # mod11
    tmp += rm32.bit # r/mビット

  # アドレス指定
  when Array
    # いまんとこ1要素限定
    raise if rm32.size != 1

    rm = rm32[0]
    case rm
    # 中身がreg
    when Register32
      # espの場合はSIBで指定しなければならない
      if ESP === rm
        # SIBバイト生成
        tmpsib = SIB.new
        tmpsib.base = rm
        sib = SIB.make_sib(tmpsib)

        tmp += 0x04 # r/mビット(SIB指定)
        tmp += 0x00 # mod00

      # ebpの場合はdisp付きじゃないとSIBが作れない
      elsif EBP === rm
        # SIBバイト生成
        tmpsib = SIB.new
        tmpsib.base = rm
        sib = SIB.make_sib(tmpsib)

        tmp += 0x04 # r/mビット(SIB指定)
        tmp += 0x80 # mod10
        
        disp = [0].pack("V")
      else
        tmp += rm.bit # r/mビット
        tmp += 0x00 # mod00
      end

    # 中身がSIBオブジェクト
    when SIB
      # dispが無い
      if rm.disp == 0
        # EBP/ESPの場合もしくはindexがある場合はSIBバイトを生成する
        if EBP === rm.base or ESP === rm.base or rm.index
          # SIBバイト生成
          sib = SIB.make_sib(rm)

          tmp += 0x04 # r/mビット(SIB指定)

        # それ以外はレジスタのビットだけ設定(SIBバイト無し)
        else
          tmp += rm.base.bit # r/mビット
        end

        # EBPでbaseだけの場合にはdisp0で生成する必要がある
        if rm.base and !rm.index and EBP === rm.base
          tmp += 0x80 # mod10
          disp = [0].pack("V")
        else
          tmp += 0x00 # mod00
        end

      # dispある
      else
        # indexがある場合はSIBバイトを生成する
        if rm.index
          # SIBバイト生成
          sib = SIB.make_sib(rm)

          tmp += 0x04 # r/mビット(SIB指定)
          tmp += 0x80 # mod10

        # EBPでindexが無い場合
        elsif EBP === rm.base
          # SIBバイト生成
          sib = SIB.make_sib(rm)

          tmp += 0x04 # r/mビット(SIB指定)
          tmp += 0x80 # mod10

        # それ以外はレジスタのビットだけ設定(SIBバイト無し)
        else
          # SIBバイト生成
          sib = SIB.make_sib(rm)

          tmp += rm.base.bit # r/mビット
          tmp += 0x80 # mod10
        end

        # disp生成
        disp = [rm.disp].pack("V")
      end

    # 中身が数字
    when Integer
      tmp += 0x05 # r/mビット
      tmp += 0x00 # mod00

      # disp生成
      disp = [rm].pack("V")
    else
      raise
    end
  else
    raise
  end
  tmp.chr + sib + disp
end
